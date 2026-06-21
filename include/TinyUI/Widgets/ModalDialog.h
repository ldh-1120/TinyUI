#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
    struct ModalState {
        bool open = false;
    };

    struct ConfirmDialogStyle {
        Color overlayColor { };

        Color backgroundColor { };
        Color borderColor { };

        Color titleTextColor { };
        Color messageTextColor { };

        Color buttonNormalColor { };
        Color buttonHoveredColor { };
        Color buttonPressedColor { };
        Color buttonBorderColor { };
        Color buttonTextColor { };

        float width = 420.0f;
        float minHeight = 190.0f;

        float radius = 14.0f;
        float borderThickness = 1.0f;

        float padding = 22.0f;
        float gap = 12.0f;

        float titleFontSize = 18.0f;
        float messageFontSize = 14.0f;

        float buttonWidth = 96.0f;
        float buttonHeight = 36.0f;
        float buttonRadius = 8.0f;

        static ConfirmDialogStyle FromTheme(const Theme& theme);
    };

    struct ConfirmDialogOptions {
        std::wstring_view confirmText = L"OK";
        std::wstring_view cancelText = L"Cancel";

        bool closeOnEscape = true;
        bool closeOnOverlayClick = false;
        bool confirmOnEnter = true;
    };

    struct ConfirmDialogResult {
        bool visible = false;
        bool confirmed = false;
        bool cancelled = false;
        bool closed = false;
    };

    void OpenModal(ModalState& state);
    void CloseModal(ModalState& state);

    ConfirmDialogResult ConfirmDialog(UIContext& context, WidgetId id, ModalState& state, std::wstring_view title, std::wstring_view message, ConfirmDialogOptions options = { });
    ConfirmDialogResult ConfirmDialog(UIContext& context, WidgetId id, ModalState& state, std::wstring_view title, std::wstring_view message, ConfirmDialogOptions options, ConfirmDialogStyle style);
}