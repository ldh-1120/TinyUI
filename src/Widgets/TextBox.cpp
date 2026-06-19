#include <TinyUI/Widgets/TextBox.h>

namespace tinyui {
	static float ClampFloat(float value, float minValue, float maxValue) {
		if (value < minValue)
			return minValue;

		if (value > maxValue)
			return maxValue;

		return value;
	}

	static std::size_t MinSize(std::size_t left, std::size_t right) {
		if (left < right)
			return left;

		return right;
	}

	static std::size_t MaxSize(std::size_t left, std::size_t right) {
		if (left > right)
			return left;

		return right;
	}

	static bool HasSelection(const tinyui::TextBoxState& state) {
		return state.selectionStart != state.selectionEnd;
	}

	static std::size_t GetSelectionStart(const tinyui::TextBoxState& state) {
		return MinSize(state.selectionStart, state.selectionEnd);
	}

	static std::size_t GetSelectionEnd(const tinyui::TextBoxState& state) {
		return MaxSize(state.selectionStart, state.selectionEnd);
	}

	static void ClearSelection(tinyui::TextBoxState& state) {
		state.selectionStart = state.cursorIndex;
		state.selectionEnd = state.cursorIndex;
	}

	static void DeleteSelection(std::wstring& value, tinyui::TextBoxState& state) {
		if (!HasSelection(state))
			return;

		std::size_t selectionStart = GetSelectionStart(state);
		std::size_t selectionEnd = GetSelectionEnd(state);

		value.erase(selectionStart, selectionEnd - selectionStart);
		
		state.cursorIndex = selectionStart;

		ClearSelection(state);
	}

	TextBoxStyle TextBoxStyle::FromTheme(const Theme& theme) {
		TextBoxStyle style {};

		style.backgroundColor = theme.textBox.background;
		style.hoveredBackgroundColor = theme.textBox.backgroundHovered;
		style.focusedBackgroundColor = theme.textBox.backgroundFocused;

		style.borderColor = theme.textBox.border;
		style.focusedBorderColor = theme.textBox.borderFocused;

		style.textColor = theme.textBox.text;
		style.placeholderColor = theme.textBox.placeholder;
		style.cursorColor = theme.textBox.cursor;
		style.selectionColor = theme.textBox.selection;

		return style;
	}

	TextBoxResult TextBox(UIContext& context, WidgetId id, TextBoxState& state, Rect rect, std::wstring& value, TextBoxOptions options, TextBoxStyle style) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();

		TextBoxResult result { };
		result.hovered = input.IsMouseOver(rect);
		result.focused = context.IsFocused(id);

		Rect textRect { rect.x + style.paddingX, rect.y, rect.w - style.paddingX * 2.f, rect.h };
		if (result.hovered)
			context.SetHoveredId(id);

		if (input.WasMousePressed(MouseButton::Left)) {
			if (result.hovered) {
				context.SetFocusedId(id);
				context.SetTextInputId(id);

				Vec2 mousePosition = input.GetMousePosition();

				float localTextX = mousePosition.x - textRect.x + state.scrollX;
				state.cursorIndex = renderer.HitTestTextPosition(value, style.fontSize, localTextX);

				ClearSelection(state);

				result.focused = true;
			} else if (context.IsFocused(id)) {
				context.ClearFocusedId();
				result.focused = false;
			}
		}

		if (state.cursorIndex > value.size())
			state.cursorIndex = value.size();

		if (state.selectionStart > value.size())
			state.selectionStart = value.size();

		if (state.selectionEnd > value.size())
			state.selectionEnd = value.size();

		if (context.IsTextInputActive(id)) {
			if (input.WasShortcutPressed(KeyCode::A, true, false, false)) {
				state.selectionStart = 0;
				state.selectionEnd = value.size();
				state.cursorIndex = value.size();
			}

			const std::wstring& textInput = input.GetTextInput();
			if (!textInput.empty()) {
				if (HasSelection(state))
					DeleteSelection(value, state);

				value.insert(state.cursorIndex, textInput);
				state.cursorIndex += textInput.size();

				ClearSelection(state);

				result.changed = true;
			}

			if (input.WasKeyPressed(KeyCode::Backspace)) {
				if (HasSelection(state)) {
					DeleteSelection(value, state);
					result.changed = true;
				} else if (state.cursorIndex > 0 && !value.empty()) {
					value.erase(state.cursorIndex - 1, 1);
					--state.cursorIndex;

					ClearSelection(state);

					result.changed = true;
				}
			}

			if (input.WasKeyPressed(KeyCode::Delete)) {
				if (HasSelection(state)) {
					DeleteSelection(value, state);
					result.changed = true;
				} else if (state.cursorIndex < value.size()) {
					value.erase(state.cursorIndex, 1);

					ClearSelection(state);

					result.changed = true;
				}
			}

			if (input.WasKeyPressed(KeyCode::Left)) {
				if (input.IsShiftDown()) {
					if (!HasSelection(state))
						state.selectionStart = state.cursorIndex;

					if (state.cursorIndex > 0)
						--state.cursorIndex;

					state.selectionEnd = state.cursorIndex;
				} else {
					if (HasSelection(state))
						state.cursorIndex = state.selectionStart;
					else if (state.cursorIndex > 0)
						--state.cursorIndex;

					ClearSelection(state);
				}
			}

			if (input.WasKeyPressed(KeyCode::Right)) {
				if (input.IsShiftDown()) {
					if (!HasSelection(state))
						state.selectionStart = state.cursorIndex;

					if (state.cursorIndex < value.size())
						++state.cursorIndex;

					state.selectionEnd = state.cursorIndex;
				} else {
					if (HasSelection(state))
						state.cursorIndex = state.selectionEnd;
					else if (state.cursorIndex < value.size())
						++state.cursorIndex;

					ClearSelection(state);
				}
			}

			if (input.WasKeyPressed(KeyCode::Home)) {
				if (input.IsShiftDown()) {
					if (!HasSelection(state))
						state.selectionStart = state.cursorIndex;

					state.cursorIndex = 0;
					state.selectionEnd = state.cursorIndex;
				} else {
					state.cursorIndex = 0;
					ClearSelection(state);
				}
			}

			if (input.WasKeyPressed(KeyCode::End)) {
				if (input.IsShiftDown()) {
					if (!HasSelection(state))
						state.selectionStart = state.cursorIndex;

					state.cursorIndex = value.size();
					state.selectionEnd = state.cursorIndex;
				} else {
					state.cursorIndex = value.size();
					ClearSelection(state);
				}
			}

			if (input.WasKeyPressed(KeyCode::Enter)) {
				if (options.submitOnEnter)
					result.submitted = true;

				if (options.clearFocusOnEnter) {
					context.ClearFocusedId();
					result.focused = false;	
				}
			}
			
			if (input.WasKeyPressed(KeyCode::Escape)) {
				if (options.clearFocusOnEscape) {
					context.ClearFocusedId();
					result.focused = false;
				}
			}
		}

		Color backgroundColor = style.backgroundColor;
		Color borderColor = style.borderColor;
		if (result.focused) {
			backgroundColor = style.focusedBackgroundColor;
			borderColor = style.focusedBorderColor;
		} else if (result.hovered)
			backgroundColor = style.hoveredBackgroundColor;

		renderer.FillRect(rect, backgroundColor, style.radius);
		renderer.DrawRect(rect, borderColor, style.borderThickness, style.radius);

		Size textSize = renderer.MeasureText(value, style.fontSize);

		std::wstring textBeforeCursor = value.substr(0, state.cursorIndex);
		Size cursorTextSize = renderer.MeasureText(textBeforeCursor, style.fontSize);

		float visibleTextWidth = textRect.w - 4.f;
		if (visibleTextWidth < 1.f)
			visibleTextWidth = 1.f;

		float maxScrollX = textSize.width - visibleTextWidth;
		if (maxScrollX < 0.f)
			maxScrollX = 0.f;

		float cursorLocalX = cursorTextSize.width;
		float cursorVisibleX = cursorLocalX - state.scrollX;

		const float cursorMargin = 4.f;
		if (result.focused) {
			if (cursorVisibleX > visibleTextWidth - cursorMargin)
				state.scrollX = cursorLocalX - visibleTextWidth + cursorMargin;

			if (cursorVisibleX < cursorMargin)
				state.scrollX = cursorLocalX - cursorMargin;
		}

		state.scrollX = ClampFloat(state.scrollX, 0.f, maxScrollX);

		renderer.PushClip(textRect);

		if (result.focused && HasSelection(state) && !value.empty()) {
			std::size_t selectionStart = GetSelectionStart(state);
			std::size_t selectionEnd = GetSelectionEnd(state);

			std::wstring textBeforeSelection = value.substr(0, selectionStart);
			std::wstring selectedText = value.substr(selectionStart, selectionEnd - selectionStart);
			Size beforeSelectionSize = renderer.MeasureText(textBeforeSelection, style.fontSize);
			Size selectedTextSize = renderer.MeasureText(selectedText, style.fontSize);

			float selectionX = textRect.x + beforeSelectionSize.width - state.scrollX;
			Rect selectionRect { selectionX, rect.y + 6.f, selectedTextSize.width, rect.h - 12.f };
			renderer.FillRect(selectionRect, style.selectionColor, 4.f);
		}

		if (!value.empty()) {
			Rect shiftedTextRect { textRect.x - state.scrollX, textRect.y, textSize.width + 32.f, textRect.h };
			renderer.DrawTextBox(value, shiftedTextRect, style.textColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);
		} else if (!options.placeholder.empty())
			renderer.DrawTextBox(options.placeholder, textRect, style.placeholderColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);

		if (result.focused) {
			float cursorX = textRect.x + cursorTextSize.width - state.scrollX;

			if (cursorX < textRect.x)
				cursorX = textRect.x;

			if (cursorX > textRect.x + textRect.w)
				cursorX = textRect.x + textRect.w;	

			Rect cursorRect { cursorX, rect.y + 8.f, 1.f, rect.h - 16.f };
			renderer.FillRect(cursorRect, style.cursorColor, 0.f);
		}

		renderer.PopClip();

		return result;
	}

	TextBoxResult TextBox(UIContext& context, Rect rect, std::wstring& value, TextBoxOptions options) {
		TextBoxState temporaryState { };

		return TextBox(context, MakeWidgetId(options.placeholder), temporaryState, rect, value, options);
	}

	TextBoxResult TextBox(UIContext& context, TextBoxState& state, Rect rect, std::wstring& value, TextBoxOptions options) {
		return TextBox(context, MakeWidgetId(options.placeholder), state, rect, value, options);
	}

	TextBoxResult TextBox(UIContext& context, WidgetId id, TextBoxState& state, Rect rect, std::wstring& value, TextBoxOptions options) {
		return TextBox(context, id, state, rect, value, options, TextBoxStyle::FromTheme(context.GetTheme()));
	}

	TextBoxResult TextBox(UIContext& context, WidgetId id, Rect rect, std::wstring& value, TextBoxOptions options) {
		TextBoxState temporaryState { };

		return TextBox(context, id, temporaryState, rect, value, options);
	}
}