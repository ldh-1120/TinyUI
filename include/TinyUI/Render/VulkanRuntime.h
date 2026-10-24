#pragma once

#include <TinyUI/Foundation/Result.h>
#include <TinyUI/Core/NativeWindow.h>
#include <TinyUI/Foundation/Handle.h>

#include <optional>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace tinyui {
    struct VulkanSurfaceTag {};
    struct VulkanDeviceTag {};

    using VulkanSurfaceId = Handle<VulkanSurfaceTag>;
    using VulkanDeviceId = Handle<VulkanDeviceTag>;

    struct VulkanDeviceInfo {
        std::string adapterName;

        std::uint32_t graphicsQueueFamily = 0;
        std::uint32_t presentQueueFamily = 0;
    };

    enum class VulkanAdapterType : uint8_t {
        Other,
        Integrated,
        Discrete,
        Virtual,
        CPU
    };

    struct VulkanAdapterInfo {
        std::string name;
        VulkanAdapterType type;

        std::uint32_t apiMajor = 0;
        std::uint32_t apiMinor = 0;
        std::uint32_t apiPatch = 0;

        bool supportsGraphics = false;
    };

    struct VulkanRuntimeOptions {
        bool enableValidation = false;

        std::vector<std::string> requiredInstanceExtensions;
    };

    class VulkanRuntime {
    public:
        [[nodiscard]] static Result<std::unique_ptr<VulkanRuntime> > create(const VulkanRuntimeOptions& options);

        ~VulkanRuntime();

        VulkanRuntime(const VulkanRuntime&) = delete;
        VulkanRuntime& operator=(const VulkanRuntime&) = delete;

        VulkanRuntime(VulkanRuntime&&) = delete;
        VulkanRuntime& operator=(VulkanRuntime&&) = delete;

        [[nodiscard]] Result<std::vector<VulkanAdapterInfo> > enumerateAdapters() const;

        [[nodiscard]] bool validationEnabled() const noexcept;

        [[nodiscard]] Result<VulkanSurfaceId> createSurface(NativeWindowHandle window);
        [[nodiscard]] bool destroySurface(VulkanSurfaceId id);

        [[nodiscard]] Result<VulkanDeviceId> createDevice(VulkanSurfaceId id);
        [[nodiscard]] bool destroyDevice(VulkanDeviceId id);

        [[nodiscard]] std::optional<VulkanDeviceInfo> deviceInfo(VulkanDeviceId id) const;

    private:
        struct Impl;

        explicit VulkanRuntime(std::unique_ptr<Impl> impl) noexcept;

        std::unique_ptr<Impl> impl_;
    };
}
