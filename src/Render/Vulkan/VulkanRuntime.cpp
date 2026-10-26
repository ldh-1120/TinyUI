#include <TinyUI/Render/VulkanRuntime.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#endif

#include <TinyUI/Render/VulkanPresenter.h>
#include <vulkan/vulkan.h>

#include <TinyUI/Foundation/SlotMap.h>

#include <array>
#include <optional>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace tinyui {
    namespace {
        Error vkError(const char* message, VkResult result) {
            return {ErrorCode::GraphicsFailure, message, static_cast<int64_t>(result)};
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
        struct Surface {
            explicit Surface(VkInstance instance) : instance(instance) {}

            ~Surface() {
                if (handle != VK_NULL_HANDLE)
                    vkDestroySurfaceKHR(instance, handle, nullptr);
            }

            VkInstance instance = VK_NULL_HANDLE;
            VkSurfaceKHR handle = VK_NULL_HANDLE;

            std::uint32_t deviceCount = 0;
        };

        struct Device {
            explicit Device(VulkanSurfaceId surface) : surface(surface) {}

            ~Device() {
                if (handle != VK_NULL_HANDLE)
                    vkDestroyDevice(handle, nullptr);
            }

            VkDevice handle = VK_NULL_HANDLE;

            VkQueue graphicsQueue = VK_NULL_HANDLE;
            VkQueue presentQueue = VK_NULL_HANDLE;

            VulkanSurfaceId surface{};
            VulkanDeviceInfo info{};

            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            std::uint32_t presenterCount = 0;
        };

        struct PresenterEntry {
            PresenterEntry(VulkanDeviceId deviceId, std::unique_ptr<detail::VulkanPresenter> value) : device(deviceId), presenter(std::move(value)) {};

            VulkanDeviceId device{};
            std::unique_ptr<detail::VulkanPresenter> presenter{};
        };

        SlotMap<PresenterEntry, VulkanPresenterTag> presenters;
        std::vector<VulkanPresenterId> presenterIds;

        VkInstance instance = VK_NULL_HANDLE;

        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugMessenger = nullptr;

        bool validationEnabled = false;
        bool win32SurfaceEnabled = false;

        SlotMap<Surface, VulkanSurfaceTag> surfaces;
        SlotMap<Device, VulkanDeviceTag> devices;

        std::vector<VulkanSurfaceId> surfaceIds;
        std::vector<VulkanDeviceId> deviceIds;

        ~Impl() {
            for (VulkanPresenterId id: presenterIds)
                presenters.erase(id);

            for (VulkanDeviceId id: deviceIds)
                devices.erase(id);

            for (VulkanSurfaceId id: surfaceIds)
                surfaces.erase(id);

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

        impl->win32SurfaceEnabled = std::ranges::find(enabledExtensions, "VK_KHR_surface") != enabledExtensions.end() && std::ranges::find(enabledExtensions, "VK_KHR_win32_surface") != enabledExtensions.end();

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

    Result<VulkanSurfaceId> VulkanRuntime::createSurface(NativeWindowHandle window) {
#if defined(_WIN32)
        if (window.system != NativeWindowSystem::Win32)
            return std::unexpected(Error{ErrorCode::Unsupported, "Unsupported native window system."});

        if (window.window == 0 || window.connection == 0)
            return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid native window handle."});

        if (!impl_->win32SurfaceEnabled)
            return std::unexpected(Error{ErrorCode::Unsupported, "Required Win32 Vulkan extensions are not enabled."});

        impl_->surfaceIds.reserve(impl_->surfaceIds.size() + 1);

        const VulkanSurfaceId id = impl_->surfaces.emplace(impl_->instance);
        Impl::Surface* surface = impl_->surfaces.get(id);

        VkWin32SurfaceCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.hwnd = reinterpret_cast<HWND>(window.window);
        info.hinstance = reinterpret_cast<HINSTANCE>(window.connection);

        const VkResult result = vkCreateWin32SurfaceKHR(impl_->instance, &info, nullptr, &surface->handle);
        if (result != VK_SUCCESS) {
            impl_->surfaces.erase(id);
            return std::unexpected(Error{ErrorCode::GraphicsFailure, "Failed to create Vulkan Win32 surface."});
        }

        impl_->surfaceIds.push_back(id);
        return id;
#else
        return std::unexpected(Error{ErrorCode::Unsupported, "Vulkan surface backend is not implemented."});
#endif
    }

    bool VulkanRuntime::destroySurface(VulkanSurfaceId id) {
        Impl::Surface* surface = impl_->surfaces.get(id);
        if (!surface || surface->deviceCount != 0)
            return false;

        std::erase(impl_->surfaceIds, id);

        return impl_->surfaces.erase(id);
    }

    Result<VulkanDeviceId> VulkanRuntime::createDevice(VulkanSurfaceId surfaceId) {
        Impl::Surface* surface = impl_->surfaces.get(surfaceId);
        if (!surface)
            return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid surface handle."});

        std::uint32_t physicalDeviceCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(impl_->instance, &physicalDeviceCount, nullptr);
        if (result != VK_SUCCESS)
            return std::unexpected(vkError("Failed to enumerate physical devices.", result));

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        result = vkEnumeratePhysicalDevices(impl_->instance, &physicalDeviceCount, physicalDevices.data());
        if (result != VK_SUCCESS)
            return std::unexpected(vkError("Failed to query physical devices.", result));

        physicalDevices.resize(physicalDeviceCount);

        struct Candidate {
            VkPhysicalDevice device = VK_NULL_HANDLE;
            std::uint32_t graphicsFamily = 0;
            std::uint32_t presentFamily = 0;
            VkPhysicalDeviceProperties properties{};
            int score = -1;
        };

        Candidate best{};
        for (VkPhysicalDevice physicalDevice: physicalDevices) {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            if (properties.apiVersion < VK_API_VERSION_1_1)
                continue;

            std::uint32_t extensionCount = 0;
            result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
            if (result != VK_SUCCESS)
                continue;

            std::vector<VkExtensionProperties> extensions(extensionCount);
            result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());
            if (result != VK_SUCCESS)
                continue;

            extensions.resize(extensionCount);

            const bool hasSwapchain = std::ranges::any_of(extensions, [](const VkExtensionProperties& extension) {
                return std::strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
            });

            if (!hasSwapchain)
                continue;

            std::uint32_t queueCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);

            std::vector<VkQueueFamilyProperties> queues(queueCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queues.data());

            std::optional<std::uint32_t> graphicsFamily;
            std::optional<std::uint32_t> presentFamily;
            std::optional<std::uint32_t> sharedFamily;
            for (std::uint32_t i = 0; i < queueCount; ++i) {
                if (queues[i].queueCount == 0)
                    continue;

                const bool graphics = (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;

                VkBool32 present = false;
                result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface->handle, &present);
                if (result != VK_SUCCESS)
                    return std::unexpected(vkError("Failed to query surface presentation support.", result));

                if (graphics && !graphicsFamily)
                    graphicsFamily = i;

                if (present == VK_TRUE && !presentFamily)
                    presentFamily = i;

                if (graphics && present == VK_TRUE && !sharedFamily)
                    sharedFamily = i;
            }

            if (!graphicsFamily || !presentFamily)
                continue;

            if (sharedFamily) {
                graphicsFamily = sharedFamily;
                presentFamily = sharedFamily;
            }

            std::uint32_t formatCount = 0;
            std::uint32_t presentModeCount = 0;
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface->handle, &formatCount, nullptr);
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to query surface formats.", result));

            result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface->handle, &presentModeCount, nullptr);
            if (result != VK_SUCCESS)
                return std::unexpected(vkError("Failed to query present modes.", result));

            if (formatCount == 0 || presentModeCount == 0)
                continue;

            int score = 0;
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 100;
            else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                score += 50;

            if (*graphicsFamily == *presentFamily)
                score += 10;

            if (score <= best.score)
                continue;

            best.device = physicalDevice;
            best.graphicsFamily = *graphicsFamily;
            best.presentFamily = *presentFamily;
            best.properties = properties;
            best.score = score;
        }

        if (best.device == VK_NULL_HANDLE)
            return std::unexpected(Error{ErrorCode::Unsupported, "No Vulkan device supports this surface."});

        impl_->deviceIds.reserve(impl_->deviceIds.size() + 1);

        const VulkanDeviceId id = impl_->devices.emplace(surfaceId);
        Impl::Device* device = impl_->devices.get(id);

        device->physicalDevice = best.device;
        device->info.adapterName = best.properties.deviceName;
        device->info.graphicsQueueFamily = best.graphicsFamily;
        device->info.presentQueueFamily = best.presentFamily;

        const float priority = 1.0f;

        std::array<std::uint32_t, 2> families{best.graphicsFamily, best.presentFamily};
        const std::uint32_t familyCount = best.graphicsFamily == best.presentFamily ? 1u : 2u;

        std::array<VkDeviceQueueCreateInfo, 2> queueInfos{};
        for (std::uint32_t i = 0; i < familyCount; ++i) {
            queueInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfos[i].queueFamilyIndex = families[i];
            queueInfos[i].queueCount = 1;
            queueInfos[i].pQueuePriorities = &priority;
        }

        const char* requiredExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.queueCreateInfoCount = familyCount;
        info.pQueueCreateInfos = queueInfos.data();
        info.enabledExtensionCount = 1;
        info.ppEnabledExtensionNames = requiredExtensions;

        result = vkCreateDevice(best.device, &info, nullptr, &device->handle);
        if (result != VK_SUCCESS) {
            impl_->devices.erase(id);
            return std::unexpected(vkError("Failed to create Vulkan logical device.", result));
        }

        vkGetDeviceQueue(device->handle, best.graphicsFamily, 0, &device->graphicsQueue);
        vkGetDeviceQueue(device->handle, best.presentFamily, 0, &device->presentQueue);

        ++surface->deviceCount;
        impl_->deviceIds.push_back(id);

        return id;
    }

    bool VulkanRuntime::destroyDevice(VulkanDeviceId id) {
        Impl::Device* device = impl_->devices.get(id);
        if (!device || device->presenterCount != 0)
            return false;

        Impl::Surface* surface = impl_->surfaces.get(device->surface);
        if (surface)
            --surface->deviceCount;

        std::erase(impl_->deviceIds, id);

        return impl_->devices.erase(id);
    }

    Result<VulkanPresenterId> VulkanRuntime::createPresenter(VulkanDeviceId deviceId, std::uint32_t pixelWidth, std::uint32_t pixelHeight) {
        Impl::Device* device = impl_->devices.get(deviceId);
        if (!device)
            return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid Vulkan device."});

        Impl::Surface* surface = impl_->surfaces.get(device->surface);
        if (!surface)
            return std::unexpected(Error{ErrorCode::InvalidArgument, "Device surface is not available."});

        detail::VulkanPresenterDesc desc{};
        desc.physicalDevice = device->physicalDevice;
        desc.device = device->handle;
        desc.surface = surface->handle;
        desc.graphicsQueue = device->graphicsQueue;
        desc.presentQueue = device->presentQueue;
        desc.graphicsFamily = device->info.graphicsQueueFamily;
        desc.presentFamily = device->info.presentQueueFamily;
        desc.width = pixelWidth;
        desc.height = pixelHeight;

        impl_->presenterIds.reserve(impl_->presenterIds.size() + 1);
        Result<std::unique_ptr<detail::VulkanPresenter> > created = detail::VulkanPresenter::create(desc);
        if (!created)
            return std::unexpected(created.error());

        const VulkanPresenterId id = impl_->presenters.emplace(deviceId, std::move(created.value()));

        impl_->presenterIds.push_back(id);
        ++device->presenterCount;

        return id;
    }

    Result<VulkanFrameStatus> VulkanRuntime::present(VulkanPresenterId id, VulkanClearColor clearColor) {
        Impl::PresenterEntry* entry = impl_->presenters.get(id);
        if (!entry)
            return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid Vulkan presenter."});

        return entry->presenter->present(clearColor);
    }

    Result<void> VulkanRuntime::resizePresenter(VulkanPresenterId id, std::uint32_t pixelWidth, std::uint32_t pixelHeight) {
        Impl::PresenterEntry* entry = impl_->presenters.get(id);
        if (!entry)
            return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid Vulkan presenter."});

        return entry->presenter->resize(pixelWidth, pixelHeight);
    }

    bool VulkanRuntime::destroyPresenter(VulkanPresenterId id) {
        Impl::PresenterEntry* entry = impl_->presenters.get(id);
        if (!entry)
            return false;

        const VulkanDeviceId deviceId = entry->device;
        if (!impl_->presenters.erase(id))
            return false;

        std::erase(impl_->presenterIds, id);

        Impl::Device* device = impl_->devices.get(deviceId);
        if (device)
            --device->presenterCount;

        return true;
    }

    std::optional<VulkanDeviceInfo> VulkanRuntime::deviceInfo(VulkanDeviceId id) const {
        const Impl::Device* device = impl_->devices.get(id);
        if (!device)
            return std::nullopt;

        return device->info;
    }
}
