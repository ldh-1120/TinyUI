#include <TinyUI/Core/Platform.h>
#include <TinyUI/Foundation/SlotMap.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <algorithm>
#include <cstdint>
#include <deque>
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tinyui {
    namespace {
        Result<std::wstring> toWide(std::string_view text) {
            if (text.empty())
                return std::wstring{};

            if (text.size() > static_cast<std::size_t>(INT_MAX))
                return std::unexpected(Error{ErrorCode::InvalidArgument, "Window title is too long."});

            const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);
            if (length == 0)
                return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid UTF-8 window title"});

            std::wstring result(length, L'\0');
            if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), result.data(), length) == 0)
                return std::unexpected(Error{ErrorCode::PlatformFailure, "UTF-8 conversion failed."});

            return result;
        }

        class Win32Platform;

        struct NativeWindow {
            explicit NativeWindow(Win32Platform* runtime) : runtime(runtime) {}

            Win32Platform* runtime;
            WindowId id{};
            HWND handle = nullptr;
            WindowMetrics metrics{};
        };

        class Win32Platform final : public Platform {
        public:
            ~Win32Platform() override {
                while (!activeWindows_.empty()) {
                    if (!destroyWindow(activeWindows_.back()))
                        std::terminate();
                }

                if (windowClass_ != 0)
                    UnregisterClassW(className_.c_str(), instance_);
            }

            Result<void> initialize() {
                instance_ = GetModuleHandleW(nullptr);
                if (!instance_)
                    return std::unexpected(Error{ErrorCode::PlatformFailure, "GetModuleHandleW failed.", static_cast<std::int64_t>(GetLastError())});

                if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
                    const DWORD error = GetLastError();
                    if (error != ERROR_ACCESS_DENIED)
                        return std::unexpected(Error{ErrorCode::PlatformFailure, "Failed to configure DPI awareness.", static_cast<std::int64_t>(error)});
                }

                className_ = L"TinyUI.Window";
                className_ += std::to_wstring(reinterpret_cast<std::uintptr_t>(this));

                WNDCLASSEXW descriptor{};
                descriptor.cbSize = sizeof(descriptor);
                descriptor.hInstance = instance_;
                descriptor.lpfnWndProc = &Win32Platform::windowProc;
                descriptor.lpszClassName = className_.c_str();
                descriptor.hCursor = LoadCursor(nullptr, IDC_ARROW);
                descriptor.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

                windowClass_ = RegisterClassExW(&descriptor);
                if (windowClass_ == 0)
                    return std::unexpected(Error{ErrorCode::PlatformFailure, "RegisterClassExW failed.", static_cast<std::int64_t>(GetLastError())});

                return {};
            }

            Result<WindowId> createWindow(const WindowOptions& options) override {
                if (options.width == 0 || options.height == 0 || options.width > 16384 || options.height > 16384)
                    return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid window dimensions."});

                Result<std::wstring> title = toWide(options.title);
                if (!title)
                    return std::unexpected(title.error());

                const WindowId id = windows_.emplace(this);
                NativeWindow* window = windows_.get(id);
                window->id = id;

                const DWORD style = WS_OVERLAPPEDWINDOW;
                const UINT dpi = GetDpiForSystem();

                RECT bounds{0, 0, MulDiv(static_cast<int>(options.width), dpi, 96), MulDiv(static_cast<int>(options.height), dpi, 96)};
                if (!AdjustWindowRectExForDpi(&bounds, style, FALSE, 0, dpi)) {
                    const DWORD error = GetLastError();
                    windows_.erase(id);

                    return std::unexpected(Error{ErrorCode::PlatformFailure, "AdjustWindowRectExForDpi failed.", static_cast<std::int64_t>(error)});
                }

                HWND handle = CreateWindowExW(0, className_.c_str(), title->c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, bounds.right - bounds.left, bounds.bottom - bounds.top, nullptr, nullptr, instance_, window);
                if (!handle) {
                    const DWORD error = GetLastError();
                    windows_.erase(id);

                    return std::unexpected(Error{ErrorCode::PlatformFailure, "CreateWindowEx failed.", static_cast<std::int64_t>(error)});
                }

                window->handle = handle;

                RECT client{};
                GetClientRect(handle, &client);

                window->metrics.pixelWidth = static_cast<std::uint32_t>(client.right);
                window->metrics.pixelHeight = static_cast<std::uint32_t>(client.bottom);
                window->metrics.scale = static_cast<float>(GetDpiForWindow(handle)) / 96.0f;

                activeWindows_.push_back(id);
                return id;
            }

            bool destroyWindow(WindowId id) override {
                NativeWindow* window = windows_.get(id);
                if (!window)
                    return false;

                if (window->handle && !DestroyWindow(window->handle))
                    return false;

                std::erase(activeWindows_, id);
                windows_.erase(id);

                return true;
            }

            bool showWindow(WindowId id) override {
                NativeWindow* window = windows_.get(id);
                if (!window || !window->handle)
                    return false;

                ShowWindow(window->handle, SW_SHOW);
                UpdateWindow(window->handle);

                return true;
            }

            std::optional<WindowMetrics> windowMetrics(WindowId id) const override {
                const NativeWindow* window = windows_.get(id);
                if (!window)
                    return std::nullopt;

                return window->metrics;
            }

            void pollEvents() override {
                MSG message{};
                while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) {
                        quitRequested_ = true;
                        continue;
                    }

                    TranslateMessage(&message);
                    DispatchMessageW(&message);
                }
            }

            void waitEvents() override {
                if (events_.empty() && !quitRequested_ && !activeWindows_.empty())
                    WaitMessage();

                pollEvents();
            }

            std::optional<PlatformEvent> nextEvent() override {
                while (!events_.empty()) {
                    PlatformEvent event = events_.front();
                    events_.pop_front();

                    if (windows_.contains(event.window))
                        return event;
                }

                return std::nullopt;
            }

            std::size_t windowCount() const noexcept override {
                return activeWindows_.size();
            }

            bool quitRequested() const noexcept override {
                return quitRequested_;
            }

        private:
            void emit(PlatformEvent event) noexcept {
                try {
                    events_.push_back(event);
                } catch (...) {
                    std::terminate();
                }
            }

            static LRESULT CALLBACK windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
                if (message == WM_NCCREATE) {
                    const CREATESTRUCTW* creation = reinterpret_cast<CREATESTRUCTW*>(lParam);

                    NativeWindow* window = static_cast<NativeWindow*>(creation->lpCreateParams);
                    window->handle = handle;

                    SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

                    return TRUE;
                }

                NativeWindow* window = reinterpret_cast<NativeWindow*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
                if (!window)
                    return DefWindowProcW(handle, message, wParam, lParam);

                Win32Platform* runtime = window->runtime;
                switch (message) {
                    case WM_CLOSE:
                        runtime->emit({window->id, PlatformEventType::CloseRequested, window->metrics});
                        return 0;

                    case WM_SIZE: {
                        RECT client{};
                        GetClientRect(handle, &client);

                        window->metrics.pixelWidth = static_cast<std::uint32_t>(client.right);
                        window->metrics.pixelHeight = static_cast<std::uint32_t>(client.bottom);

                        runtime->emit({window->id, PlatformEventType::Resized, window->metrics});
                        return 0;
                    }

                    case WM_DPICHANGED: {
                        window->metrics.scale = static_cast<float>(HIWORD(wParam)) / 96.0f;
                        runtime->emit({window->id, PlatformEventType::DpiChanged, window->metrics});

                        const RECT* suggested = reinterpret_cast<const RECT*>(lParam);
                        SetWindowPos(handle, nullptr, suggested->left, suggested->top, suggested->right - suggested->left, suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);

                        return 0;
                    }

                    case WM_PAINT: {
                        PAINTSTRUCT paint{};
                        BeginPaint(handle, &paint);
                        EndPaint(handle, &paint);

                        runtime->emit({window->id, PlatformEventType::RedrawRequested, window->metrics});

                        return 0;
                    }

                    case WM_NCDESTROY:
                        SetWindowLongPtrW(handle, GWLP_USERDATA, 0);
                        window->handle = nullptr;
                        break;
                }

                return DefWindowProcW(handle, message, wParam, lParam);
            }

            HINSTANCE instance_ = nullptr;
            ATOM windowClass_ = 0;
            std::wstring className_{};

            SlotMap<NativeWindow, WindowTag> windows_;
            std::vector<WindowId> activeWindows_;
            std::deque<PlatformEvent> events_;

            bool quitRequested_ = false;
        };
    }

    Result<std::unique_ptr<Platform> > createPlatform() {
        std::unique_ptr<Win32Platform> platform = std::make_unique<Win32Platform>();
        Result<void> result = platform->initialize();
        if (!result)
            return std::unexpected(result.error());

        return std::unique_ptr<Platform>(std::move(platform));
    }
}
