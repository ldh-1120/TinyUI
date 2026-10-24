#include <TinyUI/Render/VulkanRuntime.h>

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

int main() {
    tinyui::VulkanRuntimeOptions options{};

#ifndef NDEBUG
    options.enableValidation = true;
#endif

    tinyui::Result<std::unique_ptr<tinyui::VulkanRuntime> > created = tinyui::VulkanRuntime::create(options);
    if (!created) {
        std::cerr << created.error().message << " (Code: " << created.error().nativeCode << ")" << std::endl;
        return 1;
    }

    std::unique_ptr<tinyui::VulkanRuntime> runtime = std::move(created.value());
    std::cout << "TinyUI Vulkan Runtime initialized\n";
    std::cout << "Validation: " << (runtime->validationEnabled() ? "Enabled" : "Disabled") << std::endl;

    tinyui::Result<std::vector<tinyui::VulkanAdapterInfo> > adapters = runtime->enumerateAdapters();
    if (!adapters) {
        std::cerr << adapters.error().message << std::endl;
        return 1;
    }

    std::cout << "Physical devices: " << adapters->size() << '\n';

    for (const tinyui::VulkanAdapterInfo& adapter: *adapters) {
        std::cout << "\nDevice: " << adapter.name << "\nVulkan: " << adapter.apiMajor << "." << adapter.apiMinor << "." << adapter.apiPatch << "\nGraphics Queue: " << (adapter.supportsGraphics ? "Yes" : "No") << std::endl;
    }

    return 0;
}
