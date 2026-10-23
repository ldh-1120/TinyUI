#include <TinyUI/Core/Platform.h>

#include <iostream>
#include <memory>
#include <optional>
#include <utility>

int main() {
    using namespace tinyui;

    Result<std::unique_ptr<Platform> > created = createPlatform();
    if (!created) {
        std::cerr << created.error().message << '\n';
        return 1;
    }

    std::unique_ptr<Platform> platform = std::move(created.value());

    WindowOptions options{};
    options.title = "TinyUI Playground";
    options.width = 1280;
    options.height = 720;

    Result<WindowId> createdWindow = platform->createWindow(options);
    if (!createdWindow) {
        std::cerr << createdWindow.error().message << '\n';
        return 1;
    }

    const WindowId window = createdWindow.value();
    platform->showWindow(window);

    while (platform->windowCount() > 0 && !platform->quitRequested()) {
        platform->waitEvents();

        std::optional<PlatformEvent> event = platform->nextEvent();
        while (event) {
            switch (event->type) {
                case PlatformEventType::CloseRequested:
                    platform->destroyWindow(event->window);
                    break;

                case PlatformEventType::Resized:
                    std::cout << "Resized: " << event->metrics.pixelWidth << " x " << event->metrics.pixelHeight << '\n';
                    break;

                case PlatformEventType::DpiChanged:
                    std::cout << "DPI Scale: " << event->metrics.scale << '\n';
                    break;

                case PlatformEventType::RedrawRequested:
                    break;
            }

            event = platform->nextEvent();
        }
    }

    return 0;
}
