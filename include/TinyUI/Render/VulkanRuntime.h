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
    struct VulkanPresenterTag {};

    using VulkanSurfaceId = Handle<VulkanSurfaceTag>;
    using VulkanDeviceId = Handle<VulkanDeviceTag>;
    using VulkanPresenterId = Handle<VulkanPresenterTag>;

    struct VulkanClearColor {
        float r = 0.08f;
        float g = 0.09f;
        float b = 0.11f;
        float a = 1.0f;
    };

    enum class VulkanFrameStatus : std::uint8_t {
        Presented,
        ResizeRequired,
        Suspended
    };

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

        [[nodiscard]] Result<VulkanPresenterId> createPresenter(VulkanDeviceId device, std::uint32_t pixelWidth, std::uint32_t pixelHeight);
        [[nodiscard]] Result<VulkanFrameStatus> present(VulkanPresenterId present, VulkanClearColor clearColor);

        [[nodiscard]] Result<void> resizePresenter(VulkanPresenterId presenter, std::uint32_t pixelWidth, std::uint32_t pixelHeight);
        [[nodiscard]] bool destroyPresenter(VulkanPresenterId presenter);

    private:
        struct Impl;

        explicit VulkanRuntime(std::unique_ptr<Impl> impl) noexcept;

        std::unique_ptr<Impl> impl_;
    };
}
