#include <TinyUI/Widgets/ModalDialog.h>

#include <string>

namespace tinyui {
	static WidgetId MakeChildWidgetId(WidgetId parentId, WidgetIdValue salt) {
		WidgetIdValue value = parentId.value ^ (salt * 16777619u);
		if (value == 0)
			value = 1;

		return { value };
	}

	static void DrawDialogButton(Renderer& renderer, Rect rect, std::wstring_view text, bool hovered, bool down, const ConfirmDialogStyle& style) {
		Color backgroundColor = style.buttonNormalColor;
		if (down)
			backgroundColor = style.buttonPressedColor;
		else if (hovered)
			backgroundColor = style.buttonHoveredColor;

		renderer.FillRect(rect, backgroundColor, style.buttonRadius);
		renderer.DrawRect(rect, style.buttonBorderColor, style.borderThickness, style.buttonRadius);
		renderer.DrawTextBox(text, rect, style.buttonTextColor, 14.f, TextAlign::Center);
	}

	ConfirmDialogStyle ConfirmDialogStyle::FromTheme(const Theme& theme) {
		ConfirmDialogStyle style { };

		style.overlayColor = theme.modal.overlay;

		style.backgroundColor = theme.modal.background;
		style.borderColor = theme.modal.border;

		style.titleTextColor = theme.modal.titleText;
		style.messageTextColor = theme.modal.messageText;

		style.buttonNormalColor = theme.button.normal;
		style.buttonHoveredColor = theme.button.hovered;
		style.buttonPressedColor = theme.button.pressed;
		style.buttonBorderColor = theme.button.border;
		style.buttonTextColor = theme.button.text;

		return style;
	}

	void OpenModal(ModalState& state) {
		state.open = true;
	}

	void CloseModal(ModalState& state) {
		state.open = false;
	}

	ConfirmDialogResult ConfirmDialog(UIContext& context, WidgetId id, ModalState& state, std::wstring_view title, std::wstring_view message, ConfirmDialogOptions options) {
		return ConfirmDialog(context, id, state, title, message, options, ConfirmDialogStyle::FromTheme(context.GetTheme()));
	}

	ConfirmDialogResult ConfirmDialog(UIContext& context, WidgetId id, ModalState& state, std::wstring_view title, std::wstring_view message, ConfirmDialogOptions options, ConfirmDialogStyle style) {
		ConfirmDialogResult result { };
		if (!state.open)
			return result;

		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();

		result.visible = true;
		Size viewportSize = context.GetViewportSize();
		if (viewportSize.width <= 0.f)
			viewportSize.width = 1280.f;

		if (viewportSize.height <= 0.f)
			viewportSize.height = 720.f;

		Rect overlayRect { 0.f, 0.f, viewportSize.width, viewportSize.height };
		Size titleSize = renderer.MeasureText(title, style.titleFontSize);
		Size messageSize = renderer.MeasureText(message, style.messageFontSize);

		float contentHeight = titleSize.height + style.gap + messageSize.height + style.gap + style.buttonHeight;
		float dialogHeight = contentHeight + style.padding * 2.f;
		if (dialogHeight < style.minHeight)
			dialogHeight = style.minHeight;

		Rect dialogRect { (viewportSize.width - style.width) * 0.5f, (viewportSize.height - dialogHeight) * 0.5f, style.width, dialogHeight };
		Rect titleRect { dialogRect.x + style.padding, dialogRect.y + style.padding, dialogRect.w - style.padding * 2.f, 28.f };
		Rect messageRect { dialogRect.x + style.padding, titleRect.y + titleRect.h + style.gap, dialogRect.w - style.padding * 2.f, dialogRect.h - style.padding * 3.f - titleRect.h - style.buttonHeight };
		Rect confirmRect { dialogRect.x + dialogRect.w - style.padding - style.buttonWidth, dialogRect.y + dialogRect.h - style.padding - style.buttonHeight, style.buttonWidth, style.buttonHeight };
		Rect cancelRect { confirmRect.x - style.gap - style.buttonWidth, confirmRect.y, style.buttonWidth, style.buttonHeight };
		context.AddMouseBlocker(overlayRect);

		bool dialogHovered = input.IsMouseOver(dialogRect);
		bool confirmHovered = input.IsMouseOver(confirmRect);
		bool cancelHovered = input.IsMouseOver(cancelRect);

		WidgetId confirmId = MakeChildWidgetId(id, 1u);
		WidgetId cancelId = MakeChildWidgetId(id, 2u);
		if (confirmHovered && input.WasMousePressed(MouseButton::Left) && context.CanActive(confirmId)) {
			context.SetActiveId(confirmId, MouseButton::Left);
			context.SetFocusedId(confirmId);
			context.ClearTextInputId();
		}

		if (cancelHovered && input.WasMousePressed(MouseButton::Left) && context.CanActive(cancelId)) {
			context.SetActiveId(cancelId, MouseButton::Left);
			context.SetFocusedId(cancelId);
			context.ClearTextInputId();
		}

		bool confirmDown = context.IsActive(confirmId) && input.IsMouseDown(MouseButton::Left);
		bool cancelDown = context.IsActive(cancelId) && input.IsMouseDown(MouseButton::Left);
		if (context.IsActive(confirmId) && input.WasMouseReleased(MouseButton::Left)) {
			if (confirmHovered) {
				state.open = false;
				result.confirmed = true;
				result.closed = true;
			}

			context.ClearActiveId();
		}

		if (context.IsActive(cancelId) && input.WasMouseReleased(MouseButton::Left)) {
			if (cancelHovered) {
				state.open = false;
				result.cancelled = true;
				result.closed = true;
			}

			context.ClearActiveId();
		}

		if (options.closeOnOverlayClick && input.WasMousePressed(MouseButton::Left) && !dialogHovered) {
			state.open = false;
			result.cancelled = true;
			result.closed = true;
		}

		if (options.closeOnEscape && input.WasKeyPressed(KeyCode::Escape)) {
			state.open = false;
			result.cancelled = true;
			result.closed = true;
		}

		if (options.confirmOnEnter && input.WasKeyPressed(KeyCode::Enter)) {
			state.open = false;
			result.confirmed = true;
			result.closed = true;
		}

		std::wstring titleText(title);
		std::wstring messageText(message);
		std::wstring confirmText(options.confirmText);
		std::wstring cancelText(options.cancelText);
		context.AddOverlay([overlayRect, dialogRect, titleRect, messageRect, confirmRect, cancelRect, titleText, messageText, confirmText, cancelText, confirmHovered, cancelHovered, confirmDown, cancelDown, style](Renderer& overlayRenderer) {
			overlayRenderer.FillRect(overlayRect, style.overlayColor, 0.f);
			overlayRenderer.FillRect(dialogRect, style.backgroundColor, style.radius);
			overlayRenderer.DrawRect(dialogRect, style.borderColor, style.borderThickness, style.radius);

			std::wstring_view titleView(titleText.data(), titleText.size());
			std::wstring_view messageView(messageText.data(), messageText.size());
			std::wstring_view confirmView(confirmText.data(), confirmText.size());
			std::wstring_view cancelView(cancelText.data(), cancelText.size());

			overlayRenderer.DrawTextBox(titleView, titleRect, style.titleTextColor, style.titleFontSize, TextAlign::Left, TextWrap::NoWrap);
			overlayRenderer.DrawTextBox(messageView, messageRect, style.messageTextColor, style.messageFontSize, TextAlign::Left, TextWrap::Wrap);

			DrawDialogButton(overlayRenderer, cancelRect, cancelView, cancelHovered, cancelDown, style);
			DrawDialogButton(overlayRenderer, confirmRect, confirmView, confirmHovered, confirmDown, style);
		});

		return result;
	}
}