#include <TinyUI/Core/Platform.h>
#include <TinyUI/Render/VulkanRuntime.h>

#include <iostream>
#include <memory>
#include <optional>
#include <utility>

int main() {
    using namespace tinyui;

    Result<std::unique_ptr<Platform> > createdPlatform = createPlatform();
    if (!createdPlatform) {
        std::cerr << createdPlatform.error().message << '\n';
        return 1;
    }

    std::unique_ptr<Platform> platform = std::move(createdPlatform.value());

    WindowOptions windowOptions{};
    windowOptions.title = "TinyUI Vulkan";
    windowOptions.width = 1280;
    windowOptions.height = 720;

    Result<WindowId> createdWindow = platform->createWindow(windowOptions);
    if (!createdWindow) {
        std::cerr << createdWindow.error().message << '\n';
        return 1;
    }

    const WindowId windowId = createdWindow.value();
    std::optional<NativeWindowHandle> native = platform->nativeWindowHandle(windowId);
    if (!native)
        return 1;

    VulkanRuntimeOptions options{};
    options.requiredInstanceExtensions = {"VK_KHR_surface", "VK_KHR_win32_surface"};

#ifndef NDEBUG
    options.enableValidation = true;
#endif

    Result<std::unique_ptr<VulkanRuntime> > createdRuntime = VulkanRuntime::create(options);
    if (!createdRuntime) {
        std::cerr << createdRuntime.error().message << '\n';
        return 1;
    }

    std::unique_ptr<VulkanRuntime> runtime = std::move(createdRuntime.value());
    Result<VulkanSurfaceId> createdSurface = runtime->createSurface(*native);
    if (!createdSurface) {
        std::cerr << createdSurface.error().message << '\n';
        return 1;
    }

    const VulkanSurfaceId surfaceId = createdSurface.value();
    Result<VulkanDeviceId> createdDevice = runtime->createDevice(surfaceId);
    if (!createdDevice) {
        std::cerr << createdDevice.error().message << '\n';
        return 1;
    }

    const VulkanDeviceId deviceId = createdDevice.value();
    std::optional<VulkanDeviceInfo> device = runtime->deviceInfo(deviceId);
    if (!device)
        return 1;

    std::cout << "Vulkan initialized!\n";
    std::cout << "GPU: " << device->adapterName << '\n';
    std::cout << "Graphics Queue: " << device->graphicsQueueFamily << '\n';
    std::cout << "Present Queue: " << device->presentQueueFamily << '\n';

    platform->showWindow(windowId);

    bool running = true;

    while (running && !platform->quitRequested()) {
        platform->waitEvents();

        std::optional<PlatformEvent> event = platform->nextEvent();
        while (event) {
            if (event->type == PlatformEventType::CloseRequested)
                running = false;

            event = platform->nextEvent();
        }
    }

    const bool deviceDestroyed = runtime->destroyDevice(deviceId);
    const bool surfaceDestroyed = runtime->destroySurface(surfaceId);
    const bool windowDestroyed = platform->destroyWindow(windowId);
    if (!deviceDestroyed || !surfaceDestroyed || !windowDestroyed) {
        std::cerr << "Failed to release window resources.\n";
        return 1;
    }

    return 0;
}
