#pragma once

#include <TinyUI/Render/VulkanRuntime.h>

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>

namespace tinyui::detail {
    struct VulkanPresenterDesc {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        std::uint32_t graphicsFamily = 0;
        std::uint32_t presentFamily = 0;

        std::uint32_t width = 0;
        std::uint32_t height = 0;
    };

    class VulkanPresenter {
    public:
        [[nodiscard]] static Result<std::unique_ptr<VulkanPresenter>> create(const VulkanPresenterDesc& desc);

        ~VulkanPresenter();

        VulkanPresenter(const VulkanPresenter&) = delete;
        VulkanPresenter& operator=(const VulkanPresenter&) = delete;

        [[nodiscard]] Result<VulkanFrameStatus> present(VulkanClearColor color);
        [[nodiscard]] Result<void> resize(std::uint32_t width, std::uint32_t height);

    private:
        struct Impl;

        explicit VulkanPresenter(std::unique_ptr<Impl> impl) noexcept;

        std::unique_ptr<Impl> impl_;
    };
}