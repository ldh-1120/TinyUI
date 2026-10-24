#include <TinyUI/Render/VulkanRuntime.h>

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace tinyui {
    namespace {
        Error vkError(const char* message, VkResult result) {
            return {ErrorCode::GraphicsFailure, message, static_cast<uint32_t>(result)};
        }

        Result<std::vector<VkExtensionProperties> > getInstanceExtensions() {
            std::uint32_t count = 0;
            VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to enumerate Vulkan instance extensions.", result));

            std::vector<VkExtensionProperties> extensions(count);
            result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to query Vulkan instance extensions.", result));

            extensions.resize(count);
            return extensions;
        }

        Result<std::vector<VkLayerProperties> > getInstanceLayers() {
            std::uint32_t count = 0;
            VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr);
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to enumerate Vulkan instance layers.", result));

            std::vector<VkLayerProperties> layers(count);
            result = vkEnumerateInstanceLayerProperties(&count, layers.data());
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to query Vulkan instance layers.", result));

            layers.resize(count);
            return layers;
        }

        bool hasExtension(const std::vector<VkExtensionProperties>& extensions, const std::string& name) {
            return std::ranges::any_of(extensions, [&](const VkExtensionProperties& extension) {
                return name == extension.extensionName;
            });
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* data, void*) {
            if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && data->pMessage)
                std::fprintf(stderr, "[Vulkan] %s\n", data->pMessage);

            return VK_FALSE;
        }

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo() {
            VkDebugUtilsMessengerCreateInfoEXT info{};
            info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            info.pfnUserCallback = debugCallback;
            return info;
        }

        VulkanAdapterType adapterType(VkPhysicalDeviceType type) {
            switch (type) {
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return VulkanAdapterType::Integrated;

                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return VulkanAdapterType::Discrete;

                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    return VulkanAdapterType::Virtual;

                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    return VulkanAdapterType::CPU;

                default:
                    return VulkanAdapterType::Other;
            }
        }
    }

    struct VulkanRuntime::Impl {
        VkInstance instance = VK_NULL_HANDLE;

        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugMessenger = nullptr;

        bool validationEnabled = false;

        ~Impl() {
            if (debugMessenger != VK_NULL_HANDLE && destroyDebugMessenger)
                destroyDebugMessenger(instance, debugMessenger, nullptr);

            if (instance != VK_NULL_HANDLE)
                vkDestroyInstance(instance, nullptr);
        }
    };

    VulkanRuntime::VulkanRuntime(std::unique_ptr<Impl> impl) noexcept : impl_(std::move(impl)) {}
    VulkanRuntime::~VulkanRuntime() = default;

    bool VulkanRuntime::validationEnabled() const noexcept {
        return impl_->validationEnabled;
    }

    Result<std::unique_ptr<VulkanRuntime> > VulkanRuntime::create(const VulkanRuntimeOptions& options) {
        std::uint32_t loaderVersion = VK_API_VERSION_1_0;
        VkResult result = vkEnumerateInstanceVersion(&loaderVersion);
        if (result != VK_SUCCESS)
            return std::unexpected(vkError("Failed to query Vulkan loader version.", result));

        if (loaderVersion < VK_API_VERSION_1_1)
            return std::unexpected(vkError("Vulkan 1.1 or newer is required.", result));

        Result<std::vector<VkExtensionProperties> > availableExtensions = getInstanceExtensions();
        if (!availableExtensions)
            return std::unexpected(availableExtensions.error());

        std::vector<std::string> enabledExtensions;
        for (const std::string& name: options.requiredInstanceExtensions) {
            if (!hasExtension(*availableExtensions, name))
                return std::unexpected(Error{ErrorCode::Unsupported, "Missing Vulkan instance extension: " + name});

            if (std::ranges::find(enabledExtensions, name) == enabledExtensions.end())
                enabledExtensions.push_back(name);
        }

        const char* validationLayer = "VK_LAYER_KHRONOS_validation";
        if (options.enableValidation) {
            Result<std::vector<VkLayerProperties> > availableLayers = getInstanceLayers();
            if (!availableLayers)
                return std::unexpected(availableLayers.error());

            const bool found = std::ranges::any_of(*availableLayers, [&](const VkLayerProperties& layer) {
                return std::strcmp(layer.layerName, validationLayer) == 0;
            });

            if (!found)
                return std::unexpected(Error{ErrorCode::Unsupported, "Vulkan validation layer is not installed."});

            if (!hasExtension(*availableExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
                return std::unexpected(Error{ErrorCode::Unsupported, "VK_EXT_debug_utils is not available."});

            const std::string debugExtension = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
            if (std::ranges::find(enabledExtensions, debugExtension) == enabledExtensions.end())
                enabledExtensions.push_back(debugExtension);
        }

        VkInstanceCreateFlags flags = 0;
        if (hasExtension(*availableExtensions, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
            const std::string portabilityExtension = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
            if (std::ranges::find(enabledExtensions, portabilityExtension) == enabledExtensions.end())
                enabledExtensions.push_back(portabilityExtension);

            flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        std::vector<const char*> extensionNames;
        extensionNames.reserve(enabledExtensions.size());
        for (const std::string& name: enabledExtensions)
            extensionNames.push_back(name.c_str());

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "TinyUI";
        applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
        applicationInfo.pEngineName = "TinyUI";
        applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
        applicationInfo.apiVersion = std::min(loaderVersion, VK_API_VERSION_1_3);

        VkDebugUtilsMessengerCreateInfoEXT debugInfo = debugMessengerInfo();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.flags = flags;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensionNames.size());
        createInfo.ppEnabledExtensionNames = extensionNames.data();

        if (options.enableValidation) {
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &validationLayer;
            createInfo.pNext = &debugInfo;
        }

        std::unique_ptr<Impl> impl = std::make_unique<Impl>();
        result = vkCreateInstance(&createInfo, nullptr, &impl->instance);
        if (result != VK_SUCCESS)
            return std::unexpected(vkError("Failed to create Vulkan instance.", result));

        if (options.enableValidation) {
            PFN_vkCreateDebugUtilsMessengerEXT createMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(impl->instance, "vkCreateDebugUtilsMessengerEXT"));
            impl->destroyDebugMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(impl->instance, "vkDestroyDebugUtilsMessengerEXT"));
            if (!createMessenger || !impl->destroyDebugMessenger)
                return std::unexpected(Error{ErrorCode::GraphicsFailure, "Failed to load Vulkan debug messenger functions."});

            result = createMessenger(impl->instance, &debugInfo, nullptr, &impl->debugMessenger);
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to create Vulkan debug messenger.", result));

            impl->validationEnabled = true;
        }

        return std::unique_ptr<VulkanRuntime>(new VulkanRuntime(std::move(impl)));
    }

    Result<std::vector<VulkanAdapterInfo> > VulkanRuntime::enumerateAdapters() const {
        std::uint32_t count = 0;
        VkResult result = vkEnumeratePhysicalDevices(impl_->instance, &count, nullptr);
        if (result != VK_SUCCESS)
            return std::unexpected(vkError("Failed to enumerate physical devices.", result));

        std::vector<VkPhysicalDevice> devices(count);
        result = vkEnumeratePhysicalDevices(impl_->instance, &count, devices.data());
        if (result != VK_SUCCESS)
            return std::unexpected(vkError("Failed to query physical devices.", result));

        devices.resize(count);

        std::vector<VulkanAdapterInfo> adapters;
        adapters.reserve(devices.size());
        for (VkPhysicalDevice device: devices) {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(device, &properties);

            std::uint32_t queueCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

            std::vector<VkQueueFamilyProperties> queues(queueCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queues.data());

            queues.resize(queueCount);

            const bool supportsGraphics = std::ranges::any_of(queues, [](const VkQueueFamilyProperties& queue) {
                return queue.queueCount > 0 && (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
            });

            VulkanAdapterInfo adapter{};
            adapter.name = properties.deviceName;
            adapter.type = adapterType(properties.deviceType);

            adapter.apiMajor = VK_API_VERSION_MAJOR(properties.apiVersion);
            adapter.apiMinor = VK_API_VERSION_MINOR(properties.apiVersion);
            adapter.apiPatch = VK_API_VERSION_PATCH(properties.apiVersion);

            adapter.supportsGraphics = supportsGraphics;

            adapters.push_back(std::move(adapter));
        }

        return adapters;
    }
}
