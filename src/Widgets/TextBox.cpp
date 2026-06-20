#include <TinyUI/Widgets/TextBox.h>

#include <TinyUI/Platform/Clipboard.h>

#include <cmath>

namespace tinyui {
	enum class TextUnitKind { Whitespace, Word, Symbol };

	static bool IsWhitespace(wchar_t character) {
		return character == L' ' || character == L'\t' || character == L'\r' || character == L'\n';
	}

	static bool IsAsciiLetter(wchar_t character) {
		return (character >= L'a' && character <= L'z') || (character >= L'A' && character <= L'Z');
	}

	static bool IsAsciiDigit(wchar_t character) {
		return character >= L'0' && character <= L'9';
	}

	static TextUnitKind GetTextUnitKind(wchar_t character) {
		if (IsWhitespace(character))
			return TextUnitKind::Whitespace;

		if (IsAsciiLetter(character) || IsAsciiDigit(character) || character == L'_')
			return TextUnitKind::Word;

		return TextUnitKind::Symbol;
	}

	static std::size_t FindPreviousTextUnit(const std::wstring& value, std::size_t cursorIndex) {
		if (cursorIndex == 0)
			return 0;

		std::size_t index = cursorIndex;
		while (index > 0 && IsWhitespace(value[index - 1]))
			--index;

		if (index == 0)
			return 0;

		TextUnitKind kind = GetTextUnitKind(value[index - 1]);
		while (index > 0) {
			wchar_t character = value[index - 1];

			if (IsWhitespace(character))
				break;

			if (GetTextUnitKind(character) != kind)
				break;

			--index;
		}

		return index;
	}

	static std::size_t FindNextTextUnit(const std::wstring& value, std::size_t cursorIndex) {
		std::size_t index = cursorIndex;
		const std::size_t length = value.size();
		while (index < length && IsWhitespace(value[index]))
			++index;

		if (index >= length)
			return length;

		TextUnitKind kind = GetTextUnitKind(value[index]);
		while (index < length) {
			wchar_t character = value[index];

			if (IsWhitespace(character))
				break;

			if (GetTextUnitKind(character) != kind)
				break;

			++index;
		}

		return index;
	}

	static bool HasSelection(const tinyui::TextBoxState& state) {
		return state.selectionStart != state.selectionEnd;
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

	static std::size_t GetSelectionStart(const tinyui::TextBoxState& state) {
		return MinSize(state.selectionStart, state.selectionEnd);
	}

	static std::size_t GetSelectionEnd(const tinyui::TextBoxState& state) {
		return MaxSize(state.selectionStart, state.selectionEnd);
	}

	static void SanitizeSingleLineText(std::wstring& text) {
		std::size_t index = 0;

		while (index < text.size()) {
			wchar_t character = text[index];
			if (character == L'\r' || character == L'\n' || character == L'\t') {
				text[index] = L' ';
				++index;
				continue;
			}

			if (character < 32) {
				text.erase(index, 1);
				continue;
			}

			++index;
		}
	}

	static std::wstring GetSelectedText(const std::wstring& value, const tinyui::TextBoxState& state) {
		if (!HasSelection(state))
			return L"";

		std::size_t selectionStart = GetSelectionStart(state);
		std::size_t selectionEnd = GetSelectionEnd(state);

		return value.substr(selectionStart, selectionEnd - selectionStart);
	}

	static float ClampFloat(float value, float minValue, float maxValue) {
		if (value < minValue)
			return minValue;

		if (value > maxValue)
			return maxValue;

		return value;
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

	static std::size_t GetTextIndexFromMouse(tinyui::Renderer& renderer, const tinyui::InputState& input, const tinyui::TextBoxState& state, tinyui::Rect textRect, const std::wstring& value, float fontSize) {
		tinyui::Vec2 mousePosition = input.GetMousePosition();

		float localTextX = mousePosition.x - textRect.x + state.scrollX;
		return renderer.HitTestTextPosition(value, fontSize, localTextX);
	}

	static void ResetCursorBlink(tinyui::TextBoxState& state, const tinyui::UIContext& context) {
		state.cursorBlinkResetTime = context.GetTimeSeconds();
	}

	static bool ShouldShowCursor(const tinyui::TextBoxState& state, const tinyui::UIContext& context) {
		const double blinkInterval = 0.5;

		double elapsed = context.GetTimeSeconds() - state.cursorBlinkResetTime;
		if (elapsed < 0.)
			return true;

		double phase = std::fmod(elapsed, blinkInterval * 2.);
		return phase < blinkInterval;
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
		result.hovered = context.IsMouseOver(rect);
		result.focused = context.IsFocused(id);

		bool cursorChanged = false;

		Rect textRect { rect.x + style.paddingX, rect.y, rect.w - style.paddingX * 2.f, rect.h };
		if (result.hovered)
			context.SetHoveredId(id);

		if (input.WasMousePressed(MouseButton::Left)) {
			if (result.hovered && context.CanActive(id)) {
				context.SetFocusedId(id);
				context.SetTextInputId(id);
				context.SetActiveId(id, MouseButton::Left);

				state.cursorIndex = GetTextIndexFromMouse(renderer, input, state, textRect, value, style.fontSize);
				state.dragAnchorIndex = state.cursorIndex;
				state.selectionStart = state.cursorIndex;
				state.selectionEnd = state.cursorIndex;
				state.mouseSelecting = true;

				cursorChanged = true;
				result.focused = true;
			} else if (context.IsFocused(id)) {
				context.ClearFocusedId();
				result.focused = false;
			}
		}

		if (context.IsActive(id) && state.mouseSelecting) {
			if (input.IsMouseDown(MouseButton::Left)) {
				state.cursorIndex = GetTextIndexFromMouse(renderer, input, state, textRect, value, style.fontSize);
				state.selectionStart = state.dragAnchorIndex;
				state.selectionEnd = state.cursorIndex;
			} else {
				state.mouseSelecting = false;
				context.ClearActiveId();
			}
		}

		if (state.cursorIndex > value.size())
			state.cursorIndex = value.size();

		if (state.selectionStart > value.size())
			state.selectionStart = value.size();

		if (state.selectionEnd > value.size())
			state.selectionEnd = value.size();

		if (state.dragAnchorIndex > value.size())
			state.dragAnchorIndex = value.size();

		if (context.IsTextInputActive(id)) {
			if (input.WasShortcutPressed(KeyCode::A, true, false, false)) {
				state.selectionStart = 0;
				state.selectionEnd = value.size();
				state.cursorIndex = value.size();
			}

			if (input.WasShortcutPressed(KeyCode::C, true, false, false)) {
				if (HasSelection(state)) {
					std::wstring selectedText = GetSelectedText(value, state);

					SetClipboardText(selectedText);
				}
			}

			if (input.WasShortcutPressed(KeyCode::X, true, false, false)) {
				if (HasSelection(state)) {
					std::wstring selectedText = GetSelectedText(value, state);

					if (SetClipboardText(selectedText)) {
						DeleteSelection(value, state);
						result.changed = true;
					}
				}
			}

			if (input.WasShortcutPressed(KeyCode::V, true, false, false)) {
				std::wstring clipboardText { };
				if (GetClipboardText(clipboardText)) {
					SanitizeSingleLineText(clipboardText);
					if (!clipboardText.empty()) {
						if (HasSelection(state))
							DeleteSelection(value, state);

						value.insert(state.cursorIndex, clipboardText);

						state.cursorIndex += clipboardText.size();
						ClearSelection(state);

						result.changed = true;
					}
				}
			}

			const std::wstring& textInput = input.GetTextInput();
			if (!textInput.empty()) {
				if (HasSelection(state))
					DeleteSelection(value, state);

				value.insert(state.cursorIndex, textInput);
				state.cursorIndex += textInput.size();

				ClearSelection(state);

				cursorChanged = true;
				result.changed = true;
			}

			if (input.WasKeyPressedOrRepeated(KeyCode::Backspace)) {
				if (HasSelection(state)) {
					DeleteSelection(value, state);
					result.changed = true;
				} else if (input.IsControlDown()) {
					if (state.cursorIndex > 0 && !value.empty()) {
						std::size_t deleteStart = FindPreviousTextUnit(value, state.cursorIndex);
						if (deleteStart < state.cursorIndex) {
							value.erase(deleteStart, state.cursorIndex - deleteStart);
							state.cursorIndex = deleteStart;

							ClearSelection(state);

							cursorChanged = true;
							result.changed = true;
						}
					}
				} else if (state.cursorIndex > 0 && !value.empty()) {
					value.erase(state.cursorIndex - 1, 1);
					--state.cursorIndex;

					ClearSelection(state);

					cursorChanged = true;
					result.changed = true;
				}
			}

			if (input.WasKeyPressedOrRepeated(KeyCode::Delete)) {
				if (HasSelection(state)) {
					DeleteSelection(value, state);
					result.changed = true;
				} else if (input.IsControlDown()) {
					if (state.cursorIndex < value.size()) {
						std::size_t deleteEnd = FindNextTextUnit(value, state.cursorIndex);
						if (deleteEnd > state.cursorIndex) {
							value.erase(deleteEnd, deleteEnd - state.cursorIndex);

							ClearSelection(state);

							cursorChanged = true;
							result.changed = true;
						}
					}
				} else if (state.cursorIndex < value.size()) {
					value.erase(state.cursorIndex, 1);

					ClearSelection(state);

					cursorChanged = true;
					result.changed = true;
				}
			}

			if (input.WasKeyPressedOrRepeated(KeyCode::Left)) {
				std::size_t targetIndex = state.cursorIndex;
				if (input.IsControlDown())
					targetIndex = FindPreviousTextUnit(value, state.cursorIndex);
				else if (state.cursorIndex > 0)
					targetIndex = state.cursorIndex - 1;

				if (input.IsShiftDown()) {
					if (!HasSelection(state))
						state.selectionStart = state.cursorIndex;

					state.cursorIndex = targetIndex;
					state.selectionEnd = state.cursorIndex;
				} else {
					if (HasSelection(state))
						state.cursorIndex = GetSelectionStart(state);
					else
						state.cursorIndex = targetIndex;

					ClearSelection(state);
				}

				cursorChanged = true;
			}

			if (input.WasKeyPressedOrRepeated(KeyCode::Right)) {
				std::size_t targetIndex = state.cursorIndex;
				if (input.IsControlDown())
					targetIndex = FindNextTextUnit(value, state.cursorIndex);
				else if (state.cursorIndex < value.size())
					targetIndex = state.cursorIndex + 1;

				if (input.IsShiftDown()) {
					if (!HasSelection(state))
						state.selectionStart = state.cursorIndex;

					state.cursorIndex = targetIndex;
					state.selectionEnd = state.cursorIndex;
				} else {	
					if (HasSelection(state))
						state.cursorIndex = GetSelectionEnd(state);
					else
						state.cursorIndex = targetIndex;

					ClearSelection(state);
				}

				cursorChanged = true;
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

				cursorChanged = true;
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

				cursorChanged = true;
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
		
		if (cursorChanged)
			ResetCursorBlink(state, context);

		Color backgroundColor = style.backgroundColor;
		Color borderColor = style.borderColor;
		if (result.focused) {
			backgroundColor = style.focusedBackgroundColor;
			borderColor = style.focusedBorderColor;
		} else if (result.hovered)
			backgroundColor = style.hoveredBackgroundColor;

		renderer.FillRect(rect, backgroundColor, style.radius);
		renderer.DrawRect(rect, borderColor, style.borderThickness, style.radius);

		std::wstring displayValue = value;

		bool hasComposition = context.IsTextInputActive(id) && input.HasCompositionText();

		std::size_t displayCursorIndex = state.cursorIndex;
		std::size_t compositionInsertIndex = state.cursorIndex;
		std::size_t compositionLength = 0;
		if (hasComposition) {
			const std::wstring& compositionText = input.GetCompositionText();
			if (HasSelection(state)) {
				std::size_t selectionStart = GetSelectionStart(state);
				std::size_t selectionEnd = GetSelectionEnd(state);

				displayValue.erase(selectionStart, selectionEnd - selectionStart);

				compositionInsertIndex = selectionStart;
			}

			displayValue.insert(compositionInsertIndex, compositionText);

			compositionLength = compositionText.size();
			displayCursorIndex = compositionInsertIndex + compositionLength;
		}

		Size textSize = renderer.MeasureText(displayValue, style.fontSize);

		std::wstring textBeforeCursor = displayValue.substr(0, displayCursorIndex);
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

		if (result.focused && !hasComposition && HasSelection(state) && !value.empty()) {
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

		if (!displayValue.empty()) {
			Rect shiftedTextRect { textRect.x - state.scrollX, textRect.y, textSize.width + 32.f, textRect.h };
			renderer.DrawTextBox(displayValue, shiftedTextRect, style.textColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);
		} else if (!options.placeholder.empty())
			renderer.DrawTextBox(options.placeholder, textRect, style.placeholderColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);

		if (hasComposition && compositionLength > 0) {
			std::wstring textBeforeComposition = displayValue.substr(0, compositionInsertIndex);
			std::wstring compositionText = displayValue.substr(compositionInsertIndex, compositionLength);
			Size beforeCompositionSize = renderer.MeasureText(textBeforeComposition, style.fontSize);
			Size compositionSize = renderer.MeasureText(compositionText, style.fontSize);

			float underlineX = textRect.x + beforeCompositionSize.width - state.scrollX;
			Rect underlineRect { underlineX, rect.y + rect.h - 7.f, compositionSize.width, 1.f };
			renderer.FillRect(underlineRect, style.cursorColor, 0.f);
		}

		if (result.focused && !HasSelection(state) && ShouldShowCursor(state, context)) {
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