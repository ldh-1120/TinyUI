#include <TinyUI/Render/VulkanPresenter.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace tinyui::detail {
    namespace {
        Error graphicsError(const char* message, VkResult result) {
            return {ErrorCode::GraphicsFailure, message, static_cast<std::int64_t>(result)};
        }

        constexpr std::uint32_t FramesInFlight = 2;

        struct FrameResources {
            VkCommandBuffer commands = VK_NULL_HANDLE;
            VkSemaphore imageAvailable = VK_NULL_HANDLE;
            VkFence completed = VK_NULL_HANDLE;
        };

        struct SwapchainResources {
            explicit SwapchainResources(VkDevice device) : device(device) {}

            ~SwapchainResources() {
                for (VkSemaphore semaphore: renderFinished) {
                    if (semaphore != VK_NULL_HANDLE)
                        vkDestroySemaphore(device, semaphore, nullptr);
                }

                if (handle != VK_NULL_HANDLE)
                    vkDestroySwapchainKHR(device, handle, nullptr);
            }

            VkDevice device = VK_NULL_HANDLE;
            VkSwapchainKHR handle = VK_NULL_HANDLE;
            VkExtent2D extent{};

            std::vector<VkImage> images;
            std::vector<VkSemaphore> renderFinished;
        };
    }

    struct VulkanPresenter::Impl {
        explicit Impl(const VulkanPresenterDesc& description) : desc(description) {};

        ~Impl() {
            if (desc.device != VK_NULL_HANDLE)
                vkDeviceWaitIdle(desc.device);

            swapchain.reset();
            for (FrameResources& frame: frames) {
                if (frame.imageAvailable != VK_NULL_HANDLE)
                    vkDestroySemaphore(desc.device, frame.imageAvailable, nullptr);

                if (frame.completed != VK_NULL_HANDLE)
                    vkDestroyFence(desc.device, frame.completed, nullptr);
            }

            if (commandPool != VK_NULL_HANDLE)
                vkDestroyCommandPool(desc.device, commandPool, nullptr);
        }

        Result<void> initializeFrames();
        Result<void> recreate(std::uint32_t width, std::uint32_t height);
        Result<VulkanFrameStatus> draw(VulkanClearColor clear);

        VulkanPresenterDesc desc;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::array<FrameResources, FramesInFlight> frames{};

        std::unique_ptr<SwapchainResources> swapchain;

        std::uint32_t frameIndex = 0;
        bool suspended = false;
        bool failed = false;
    };

    VulkanPresenter::VulkanPresenter(std::unique_ptr<VulkanPresenter::Impl> impl) noexcept : impl_(std::move(impl)) {}
    VulkanPresenter::~VulkanPresenter() = default;

    Result<std::unique_ptr<VulkanPresenter> > VulkanPresenter::create(const VulkanPresenterDesc& desc) {
        std::unique_ptr<Impl> impl = std::make_unique<Impl>(desc);
        Result<void> frames = impl->initializeFrames();
        if (!frames)
            return std::unexpected(frames.error());

        Result<void> swapchain = impl->recreate(desc.width, desc.height);
        if (!swapchain)
            return std::unexpected(swapchain.error());

        return std::unique_ptr<VulkanPresenter>(new VulkanPresenter(std::move(impl)));
    }

    Result<void> VulkanPresenter::resize(std::uint32_t width, std::uint32_t height) {
        if (impl_->failed)
            return std::unexpected(Error{ErrorCode::GraphicsFailure, "Presenter is in a failed state."});

        return impl_->recreate(width, height);
    }

    Result<VulkanFrameStatus> VulkanPresenter::present(VulkanClearColor clear) {
        if (impl_->failed)
            return std::unexpected(Error{ErrorCode::GraphicsFailure, "Presenter is in a failed state."});

        return impl_->draw(clear);
    }

    Result<void> VulkanPresenter::Impl::initializeFrames() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = desc.graphicsFamily;

        VkResult result = vkCreateCommandPool(desc.device, &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to create command pool.", result));

        VkCommandBufferAllocateInfo allocation{};
        allocation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocation.commandPool = commandPool;
        allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocation.commandBufferCount = FramesInFlight;

        std::array<VkCommandBuffer, FramesInFlight> buffers{};
        result = vkAllocateCommandBuffers(desc.device, &allocation, buffers.data());
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to allocate command buffers.", result));

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (std::uint32_t i = 0; i < FramesInFlight; i++) {
            FrameResources& frame = frames[i];
            frame.commands = buffers[i];

            result = vkCreateSemaphore(desc.device, &semaphoreInfo, nullptr, &frame.imageAvailable);
            if (result != VK_SUCCESS)
                return std::unexpected(graphicsError("Failed to create acquire semaphore.", result));

            result = vkCreateFence(desc.device, &fenceInfo, nullptr, &frame.completed);
            if (result != VK_SUCCESS)
                return std::unexpected(graphicsError("Failed to create frame fence.", result));
        }

        return {};
    }

    Result<void> VulkanPresenter::Impl::recreate(std::uint32_t width, std::uint32_t height) {
        if (width == 0 || height == 0) {
            suspended = true;
            return {};
        }

        VkSurfaceCapabilitiesKHR capabilities{};
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(desc.physicalDevice, desc.surface, &capabilities);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to query surface capabilities.", result));

        VkExtent2D extent{};
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            extent = capabilities.currentExtent;
        else {
            extent.width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            extent.height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        }

        if (extent.width == 0 || extent.height == 0) {
            suspended = true;
            return {};
        }

        if (!(capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
            return std::unexpected(Error{ErrorCode::Unsupported, "Surface does not support transfer destination images."});

        std::uint32_t formatCount = 0;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(desc.physicalDevice, desc.surface, &formatCount, nullptr);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to enumerate surface formats.", result));

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(desc.physicalDevice, desc.surface, &formatCount, formats.data());
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to query surface formats.", result));

        formats.resize(formatCount);

        VkSurfaceFormatKHR chosenFormat{};
        bool foundFormat = false;

        for (const VkSurfaceFormatKHR& format: formats) {
            const VkFormat candidateFormat = format.format == VK_FORMAT_UNDEFINED ? VK_FORMAT_B8G8R8A8_UNORM : format.format;

            VkFormatProperties properties{};
            vkGetPhysicalDeviceFormatProperties(desc.physicalDevice, candidateFormat, &properties);
            if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT))
                continue;

            if (!foundFormat) {
                chosenFormat = format;
                chosenFormat.format = candidateFormat;
                foundFormat = true;
            }

            if (candidateFormat == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                chosenFormat = format;
                chosenFormat.format = candidateFormat;
                break;
            }
        }

        if (!foundFormat)
            return std::unexpected(Error{ErrorCode::Unsupported, "No supported swapchain format for clearing."});

        std::uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount != 0)
            imageCount = std::min(imageCount, capabilities.maxImageCount);

        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        if (!(capabilities.supportedCompositeAlpha & compositeAlpha)) {
            constexpr std::array<VkCompositeAlphaFlagBitsKHR, 3> choices{VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR};
            for (VkCompositeAlphaFlagBitsKHR choice : choices) {
                if (capabilities.supportedCompositeAlpha & choice) {
                    compositeAlpha = choice;
                    break;
                }
            }
        }

        if (!(capabilities.supportedCompositeAlpha & compositeAlpha))
            return std::unexpected(Error{ErrorCode::Unsupported, "No supported composite alpha mode."});

        const std::array<std::uint32_t, 2> queueFamilies{desc.graphicsFamily, desc.presentFamily};

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = desc.surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = chosenFormat.format;
        createInfo.imageColorSpace = chosenFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = compositeAlpha;
        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;

        if (desc.graphicsFamily != desc.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilies.data();
        } else
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        createInfo.oldSwapchain = swapchain ? swapchain->handle : VK_NULL_HANDLE;
        result = vkDeviceWaitIdle(desc.device);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to wait for device idle.", result));

        std::unique_ptr<SwapchainResources> next = std::make_unique<SwapchainResources>(desc.device);
        result = vkCreateSwapchainKHR(desc.device, &createInfo, nullptr, &next->handle);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to create swapchain.", result));

        std::uint32_t actualImageCount = 0;
        result = vkGetSwapchainImagesKHR(desc.device, next->handle, &actualImageCount, nullptr);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to enumerate swapchain images.", result));

        next->images.resize(actualImageCount);
        result = vkGetSwapchainImagesKHR(desc.device, next->handle, &actualImageCount, next->images.data());
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to retrieve swapchain images.", result));

        next->images.resize(actualImageCount);
        next->renderFinished.resize(actualImageCount, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (VkSemaphore& semaphore : next->renderFinished) {
            result = vkCreateSemaphore(desc.device, &semaphoreInfo, nullptr, &semaphore);
            if (result != VK_SUCCESS)
                return std::unexpected(graphicsError("Failed to create presentation semaphore.", result));
        }

        next->extent = extent;

        swapchain = std::move(next);
        suspended = false;

        return {};
    }

    Result<VulkanFrameStatus> VulkanPresenter::Impl::draw(VulkanClearColor clear) {
        if (suspended || !swapchain)
            return VulkanFrameStatus::Suspended;

        FrameResources& frame = frames[frameIndex];
        VkResult result = vkWaitForFences(desc.device, 1, &frame.completed, VK_TRUE, UINT64_MAX);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to wait for frame fences.", result));

        std::uint32_t imageIndex = 0;
        result = vkAcquireNextImageKHR(desc.device, swapchain->handle, UINT64_MAX, frame.imageAvailable, VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            return VulkanFrameStatus::ResizeRequired;

        const bool suboptimal = result == VK_SUBOPTIMAL_KHR;
        if (result != VK_SUCCESS && !suboptimal)
            return std::unexpected(graphicsError("Failed to acquire swapchain image.", result));

        failed = true;
        result = vkResetCommandBuffer(frame.commands, 0);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to reset command buffer.", result));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = vkBeginCommandBuffer(frame.commands, &beginInfo);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to begin command buffer.", result));

        VkImageSubresourceRange range{};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        VkImageMemoryBarrier toTransfer{};
        toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toTransfer.srcAccessMask = 0;
        toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.image = swapchain->images[imageIndex];
        toTransfer.subresourceRange = range;

        vkCmdPipelineBarrier(frame.commands, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTransfer);

        VkClearColorValue clearValue{};
        clearValue.float32[0] = clear.r;
        clearValue.float32[1] = clear.g;
        clearValue.float32[2] = clear.b;
        clearValue.float32[3] = clear.a;

        vkCmdClearColorImage(frame.commands, swapchain->images[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &range);

        VkImageMemoryBarrier toPresent{};
        toPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        toPresent.dstAccessMask = 0;
        toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        toPresent.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toPresent.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toPresent.image = swapchain->images[imageIndex];
        toPresent.subresourceRange = range;

        vkCmdPipelineBarrier(frame.commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &toPresent);

        result = vkEndCommandBuffer(frame.commands);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to end command buffer.", result));

        result = vkResetFences(desc.device, 1, &frame.completed);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to reset frame fence.", result));

        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        const VkSemaphore renderFinished = swapchain->renderFinished[imageIndex];

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &frame.imageAvailable;
        submit.pWaitDstStageMask = &waitStage;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &frame.commands;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &renderFinished;

        result = vkQueueSubmit(desc.graphicsQueue, 1, &submit, frame.completed);
        if (result != VK_SUCCESS)
            return std::unexpected(graphicsError("Failed to submit GPU commands.", result));

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain->handle;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(desc.presentQueue, &presentInfo);
        frameIndex = (frameIndex + 1) % FramesInFlight;

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            failed = false;
            return VulkanFrameStatus::ResizeRequired;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            return std::unexpected(graphicsError("Failed to present swapchain image.", result));

        failed = false;

        if (suboptimal || result == VK_SUBOPTIMAL_KHR)
            return VulkanFrameStatus::ResizeRequired;

        return VulkanFrameStatus::Presented;
    }
}
