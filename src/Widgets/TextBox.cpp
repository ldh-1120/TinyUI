#include <pch.h>

#include <TinyCore/Math/Numeric.h>
#include <TinyCore/Platform/Clipboard.h>
#include <TinyUI/Widgets/TextBox.h>
#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>

#include <cwctype>

namespace tinyui {
	TextBox::TextBox() {
		GetLayoutStyle().preferredSize = { 180.0f, 34.0f };
		GetLayoutStyle().verticalAlignment = LayoutAlignment::Center;
	}

	void TextBox::SetText(std::wstring_view text) {
		SetTextInternal(text, false);
	}

	const std::wstring& TextBox::GetText() const {
		return m_text;
	}

	void TextBox::SetOptions(const TextBoxOptions& options) {
		m_options = options;
	}

	const TextBoxOptions& TextBox::GetOptions() const {
		return m_options;
	}

	bool TextBox::HasPendingChange() const {
		return m_changed;
	}

	bool TextBox::TakeChanged() {
		bool changed = m_changed;
		m_changed = false;

		return changed;
	}

	bool TextBox::HasSelection() const {
		return m_caretIndex != m_selectionAnchorIndex;
	}

	std::size_t TextBox::GetSelectionStart() const {
		if (m_caretIndex < m_selectionAnchorIndex)
			return m_caretIndex;

		return m_selectionAnchorIndex;
	}

	std::size_t TextBox::GetSelectionEnd() const {
		if (m_caretIndex > m_selectionAnchorIndex)
			return m_caretIndex;

		return m_selectionAnchorIndex;
	}

	bool TextBox::IsMouseInteractive() const {
		return true;
	}

	bool TextBox::IsFocusable() const {
		return true;
	}

	bool TextBox::ShouldDrawFocusRing() const {
		return false;
	}

	bool TextBox::GetImeCandidatePosition(tinycore::Vec2& position) const {
		if (!IsFocused() || !m_hasImeCandidatePosition)
			return false;

		position = m_imeCandidatePosition;

		return true;
	}

	void TextBox::OnPaint(PaintContext& context) {
		const Theme& theme = context.theme;

		tinycore::Rect rect = GetRect();
		tinycore::Color background = tinycore::LerpColor(theme.textBox.background, theme.textBox.backgroundHovered, m_hoverT);
		background = tinycore::LerpColor(background, theme.textBox.backgroundFocused, m_focusT);

		tinycore::Color border = tinycore::LerpColor(theme.textBox.border, theme.textBox.borderHovered, m_hoverT);
		border = tinycore::LerpColor(border, theme.textBox.borderFocused, m_focusT);

		context.renderer.FillRect(rect, background, theme.textBox.cornerRadius);
		context.renderer.DrawRect(rect, border, theme.textBox.borderThickness, theme.textBox.cornerRadius);

		tinycore::Rect textRect = GetTextRect(theme);
		std::wstring displayText = GetDisplayText();
		EnsureCaretVisible(context.renderer, theme, displayText, GetDisplayCaretIndex(), textRect);

		const std::wstring& visibleText = displayText.empty() ? m_options.placeholder : displayText;
		tinycore::Color textColor = displayText.empty() ? theme.textBox.placeholder : theme.textBox.text;
		tinycore::Rect drawTextRect = displayText.empty() ? textRect : GetScrolledTextRect(context.renderer, theme, displayText, textRect);

		context.renderer.PushClip(textRect);

		PaintSelection(context, textRect);
		context.renderer.DrawTextBox(visibleText, drawTextRect, textColor, theme.textBox.fontSize, TextAlign::Left, TextWrap::NoWrap);
		PaintCompositionUnderline(context, textRect);

		context.renderer.PopClip();

		if (IsFocused()) {
			float caretX = GetCaretX(context, textRect);
			m_imeCandidatePosition = { caretX, textRect.y + textRect.h };
			m_hasImeCandidatePosition = true;

			int blinkPhase = static_cast<int>(m_caretBlinkTimer / 0.5f) % 2;
			if (blinkPhase == 0)
				context.renderer.DrawLine({ caretX, textRect.y + 3.f }, { caretX, textRect.y + textRect.h - 3.f }, theme.textBox.caret, 1.f);
		} else
			m_hasImeCandidatePosition = false;
	}

	bool TextBox::OnUpdate(float deltaTime) {
		float previousHoverT = m_hoverT;
		float previousFocusT = m_focusT;
		float previousBlinkTimer = m_caretBlinkTimer;

		float hoverTarget = IsHovered() ? 1.0f : 0.0f;
		float focusTarget = IsFocused() ? 1.0f : 0.0f;

		m_hoverT = tinycore::MoveTowards(m_hoverT, hoverTarget, 12.0f * deltaTime);
		m_focusT = tinycore::MoveTowards(m_focusT, focusTarget, 14.0f * deltaTime);
		if (IsFocused())
			m_caretBlinkTimer += deltaTime;
		else
			m_caretBlinkTimer = 0.0f;

		return previousHoverT != m_hoverT || previousFocusT != m_focusT || previousBlinkTimer != m_caretBlinkTimer;
	}

	void TextBox::OnBlur() {
		CommitCompositionText();
		m_selecting = false;
	}

	void TextBox::OnMouseDown(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left)
			return;

		if (!event.theme || !event.renderer) {
			SetCaretIndex(m_text.size(), false);

			event.Accept();
			return;
		}

		std::size_t index = HitTestCaretIndex(*event.renderer, *event.theme, event.position);
		SetCaretIndex(index, false);

		m_selecting = true;

		event.Accept();
	}

	void TextBox::OnMouseMove(MouseEvent& event) {
		if (!m_selecting || !event.theme || !event.renderer)
			return;

		std::size_t index = HitTestCaretIndex(*event.renderer, *event.theme, event.position);
		SetCaretIndex(index, true);

		event.Accept();
	}

	void TextBox::OnMouseUp(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left || !m_selecting)
			return;

		m_selecting = false;

		event.Accept();
	}

	void TextBox::OnKeyDown(KeyEvent& event) {
		if (event.controlDown) {
			if (event.key == tinycore::KeyCode::A) {
				SelectAll();

				event.Accept();
				return;
			}

			if (event.key == tinycore::KeyCode::C) {
				CopySelectionToClipboard();

				event.Accept();
				return;
			}

			if (event.key == tinycore::KeyCode::X) {
				CutSelectionToClipboard();

				event.Accept();
				return;
			}

			if (event.key == tinycore::KeyCode::V) {
				PasteFromClipboard();

				event.Accept();
				return;
			}
		}

		if (event.key == tinycore::KeyCode::Left) {
			if (event.controlDown)
				MoveCaretPreviousWord(event.shiftDown);
			else
				MoveCaretLeft(event.shiftDown);

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::Right) {
			if (event.controlDown)
				MoveCaretNextWord(event.shiftDown);
			else
				MoveCaretRight(event.shiftDown);

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::Home) {
			MoveCaretHome(event.shiftDown);

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::End) {
			MoveCaretEnd(event.shiftDown);

			event.Accept();
			return;
		}

		if (m_options.readOnly)
			return;

		if (event.key == tinycore::KeyCode::Backspace) {
			if (event.controlDown)
				DeletePreviousWord();
			else
				Backspace();

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::Delete) {
			if (event.controlDown)
				DeleteNextWord();
			else
				Delete();

			event.Accept();
			return;
		}
	}

	void TextBox::Backspace() {
		if (DeleteSelection() || m_caretIndex == 0)
			return;

		m_text.erase(m_text.begin() + static_cast<std::ptrdiff_t>(m_caretIndex - 1));

		--m_caretIndex;
		m_selectionAnchorIndex = m_caretIndex;

		m_changed = true;
		m_caretBlinkTimer = 0.f;
	}

	void TextBox::Delete() {
		if (DeleteSelection() || m_caretIndex >= m_text.size())
			return;

		m_text.erase(m_text.begin() + static_cast<std::ptrdiff_t>(m_caretIndex));
		m_selectionAnchorIndex = m_caretIndex;

		m_changed = true;
		m_caretBlinkTimer = 0.f;
	}

	void TextBox::MoveCaretLeft(bool keepSelection) {
		if (m_caretIndex > 0)
			--m_caretIndex;

		if (!keepSelection)
			m_selectionAnchorIndex = m_caretIndex;

		m_caretBlinkTimer = 0.f;
	}

	void TextBox::MoveCaretRight(bool keepSelection) {
		if (m_caretIndex < m_text.size())
			++m_caretIndex;

		if (!keepSelection)
			m_selectionAnchorIndex = m_caretIndex;

		m_caretBlinkTimer = 0.f;
	}

	void TextBox::MoveCaretHome(bool keepSelection) {
		m_caretIndex = 0;
		if (!keepSelection)
			m_selectionAnchorIndex = m_caretIndex;

		m_caretBlinkTimer = 0.f;
	}

	void TextBox::MoveCaretEnd(bool keepSelection) {
		m_caretIndex = m_text.size();
		if (!keepSelection)
			m_selectionAnchorIndex = m_caretIndex;

		m_caretBlinkTimer = 0.f;
	}
	
	void TextBox::OnTextInput(TextInputEvent& event) {
		if (m_options.readOnly || !IsEditableCharacter(event.character))
			return;

		if (!m_compositionText.empty())
			m_compositionText.clear();
		
		InsertCharacter(event.character);
		event.Accept();
	}

	void TextBox::OnTextComposition(TextCompositionEvent& event) {
		m_compositionText = event.text;
		event.Accept();
	}

	void TextBox::SetTextInternal(std::wstring_view text, bool notifyChanged) {
		if (m_text == text)
			return;

		m_text = text;
		if (m_caretIndex > m_text.size())
			m_caretIndex = m_text.size();

		if (m_selectionAnchorIndex > m_text.size())
			m_selectionAnchorIndex = m_text.size();

		if (notifyChanged)
			m_changed = true;
	}

	void TextBox::InsertCharacter(wchar_t character) {
		if (HasSelection())
			DeleteSelection();

		m_text.insert(m_text.begin() + static_cast<std::ptrdiff_t>(m_caretIndex), character);

		++m_caretIndex;
		m_selectionAnchorIndex = m_caretIndex;

		m_changed = true;
		m_caretBlinkTimer = 0.f;
	}

	bool TextBox::IsEditableCharacter(wchar_t character) const {
		if (character < 32)
			return false;

		return true;
	}

	tinycore::Rect TextBox::GetTextRect(const Theme& theme) const {
		tinycore::Rect rect = GetRect();
		tinycore::Thickness padding = theme.textBox.padding;

		return { rect.x + padding.left, rect.y + padding.top, rect.w - padding.left - padding.right, rect.h - padding.bottom - padding.top };
	}

	float TextBox::GetCaretX(PaintContext& context, tinycore::Rect textRect) const {
		std::wstring displayText = GetDisplayText();
		float caretLocalX = GetCaretLocalX(context.renderer, context.theme, displayText, GetDisplayCaretIndex(), textRect);

		return textRect.x + caretLocalX - m_scrollX;
	}

	void TextBox::SetCaretIndex(std::size_t index, bool keepSelection) {
		if (index > m_text.size())
			index = m_text.size();

		m_caretIndex = index;
		if (!keepSelection)
			m_selectionAnchorIndex = m_caretIndex;

		m_caretBlinkTimer = 0.0f;
	}

	void TextBox::ClearSelection() {
		m_selectionAnchorIndex = m_caretIndex;
	}

	bool TextBox::DeleteSelection() {
		if (!HasSelection())
			return false;

		std::size_t start = GetSelectionStart();
		std::size_t end = GetSelectionEnd();
		m_text.erase(m_text.begin() + static_cast<std::ptrdiff_t>(start), m_text.begin() + static_cast<std::ptrdiff_t>(end));

		m_caretIndex = start;
		m_selectionAnchorIndex = start;

		m_changed = true;
		m_caretBlinkTimer = 0.0f;

		return true;
	}

	std::size_t TextBox::HitTestCaretIndex(Renderer& renderer, const Theme& theme, tinycore::Vec2 position) const {
		tinycore::Rect textRect = GetTextRect(theme);
		if (m_text.empty())
			return 0;

		tinycore::Vec2 localPosition = GetTextLocalPosition(position, textRect);
		float layoutWidth = GetTextLayoutWidth(renderer, theme, m_text, textRect);

		return renderer.HitTestTextIndex(m_text, { layoutWidth, textRect.h }, theme.textBox.fontSize, TextWrap::NoWrap, localPosition);
	}

	void TextBox::PaintSelection(PaintContext& context, tinycore::Rect textRect) {
		if (!HasSelection())
			return;

		std::size_t start = GetSelectionStart();
		std::size_t end = GetSelectionEnd();
		std::size_t length = end - start;
		float layoutWidth = GetTextLayoutWidth(context.renderer, context.theme, m_text, textRect);
		std::vector<tinycore::Rect> rects = context.renderer.HitTestTextRange(m_text, start, length, { layoutWidth, textRect.h }, context.theme.textBox.fontSize, TextWrap::NoWrap);
		for (std::size_t index = 0; index < rects.size(); ++index) {
			tinycore::Rect rect = rects[index];

			rect.x += textRect.x - m_scrollX;
			rect.y += textRect.y;
			context.renderer.FillRect(rect, context.theme.textBox.selection, 3.f);
		}
	}

	bool TextBox::IsWordCharacter(wchar_t character) const {
		if (std::iswspace(character) || std::iswpunct(character))
			return false;

		return true;
	}

	std::size_t TextBox::FindPreviousWordBoundary(std::size_t index) const {
		if (index == 0)
			return 0;

		std::size_t current = index;
		while (current > 0 && !IsWordCharacter(m_text[current - 1]))
			--current;

		while (current > 0 && IsWordCharacter(m_text[current - 1]))
			--current;

		return current;
	}

	std::size_t TextBox::FindNextWordBoundary(std::size_t index) const {
		std::size_t current = index;
		std::size_t textSize = m_text.size();
		if (current >= textSize)
			return textSize;

		while (current < textSize && IsWordCharacter(m_text[current]))
			++current;

		while (current < textSize && !IsWordCharacter(m_text[current]))
			++current;

		return current;
	}

	void TextBox::MoveCaretPreviousWord(bool keepSelection) {
		std::size_t index = FindPreviousWordBoundary(m_caretIndex);
		SetCaretIndex(index, keepSelection);
	}

	void TextBox::MoveCaretNextWord(bool keepSelection) {
		std::size_t index = FindNextWordBoundary(m_caretIndex);
		SetCaretIndex(index, keepSelection);
	}

	void TextBox::DeletePreviousWord() {
		if (DeleteSelection() || m_caretIndex == 0)
			return;

		std::size_t start = FindPreviousWordBoundary(m_caretIndex);
		if (start == m_caretIndex)
			return;

		m_text.erase(m_text.begin() + static_cast<std::ptrdiff_t>(start), m_text.begin() + static_cast<std::ptrdiff_t>(m_caretIndex));
		
		m_caretIndex = start;
		m_selectionAnchorIndex = start;

		m_changed = true;
		m_caretBlinkTimer = 0.f;
	}

	void TextBox::DeleteNextWord() {
		if (DeleteSelection() || m_caretIndex >= m_text.size())
			return;

		std::size_t end = FindNextWordBoundary(m_caretIndex);
		if (end == m_caretIndex)
			return;

		m_text.erase(m_text.begin() + static_cast<std::ptrdiff_t>(m_caretIndex), m_text.begin() + static_cast<std::ptrdiff_t>(end));

		m_selectionAnchorIndex = m_caretIndex;

		m_changed = true;
		m_caretBlinkTimer = 0.f;
	}

	void TextBox::SelectAll() {
		m_selectionAnchorIndex = 0;
		m_caretIndex = m_text.size();
		m_caretBlinkTimer = 0.f;
	}

	std::wstring TextBox::GetSelectedText() const {
		if (!HasSelection())
			return L"";

		std::size_t start = GetSelectionStart();
		std::size_t end = GetSelectionEnd();

		return m_text.substr(start, end - start);
	}

	void TextBox::CopySelectionToClipboard() {
		if (!HasSelection())
			return;

		std::wstring selectedText = GetSelectedText();
		if (selectedText.empty())
			return;

		tinycore::SetClipboardText(selectedText);
	}

	void TextBox::CutSelectionToClipboard() {
		if (m_options.readOnly || !HasSelection())
			return;

		CopySelectionToClipboard();
		DeleteSelection();
	}

	void TextBox::PasteFromClipboard() {
		if (m_options.readOnly)
			return;

		std::wstring clipboardText { };
		if (!tinycore::GetClipboardText(clipboardText))
			return;

		if (clipboardText.empty())
			return;

		if (HasSelection())
			DeleteSelection();

		m_text.insert(m_caretIndex, clipboardText);

		m_caretIndex += clipboardText.size();
		m_selectionAnchorIndex = m_caretIndex;

		m_changed = true;
		m_caretBlinkTimer = 0.f;
	}

	std::wstring TextBox::GetDisplayText() const {
		if (m_compositionText.empty())
			return m_text;

		std::wstring displayText = m_text;
		displayText.insert(m_caretIndex, m_compositionText);

		return displayText;
	}
	std::size_t TextBox::GetDisplayCaretIndex() const {
		return m_caretIndex + m_compositionText.size();
	}

	void TextBox::PaintCompositionUnderline(PaintContext& context, tinycore::Rect textRect) {
		if (m_compositionText.empty())
			return;

		std::wstring displayText = GetDisplayText();
		std::size_t start = m_caretIndex;
		std::size_t length = m_compositionText.size();
		std::vector<tinycore::Rect> rects = context.renderer.HitTestTextRange(displayText, start, length, { textRect.w, textRect.h }, context.theme.textBox.fontSize, TextWrap::NoWrap);
		for (std::size_t index = 0; index < rects.size(); ++index) {
			tinycore::Rect rect = rects[index];
			
			float x = textRect.x + rect.x - m_scrollX;
			float y = textRect.y + rect.y + rect.h - 2.0f;
			context.renderer.DrawLine({ x, y }, { x + rect.w, y }, context.theme.textBox.compositionUnderline, 1.f);
		}
	}

	bool TextBox::CommitCompositionText() {
		if (m_compositionText.empty())
			return false;

		if (m_options.readOnly) {
			m_compositionText.clear();
			return false;
		}

		if (HasSelection())
			DeleteSelection();

		m_text.insert(m_caretIndex, m_compositionText);
		m_caretIndex += m_compositionText.size();
		m_selectionAnchorIndex = m_caretIndex;

		m_compositionText.clear();

		m_changed = true;
		m_caretBlinkTimer = 0.f;

		return true;
	}

	float TextBox::GetTextLayoutWidth(Renderer& renderer, const Theme& theme, std::wstring_view text, tinycore::Rect textRect) const {
		if (text.empty())
			return textRect.w;

		tinycore::Size textSize = renderer.MeasureText(text, theme.textBox.fontSize, TextWrap::NoWrap);
		float layoutWidth = textSize.width + 32.f;
		if (layoutWidth < textRect.w)
			layoutWidth = textRect.w;

		return layoutWidth;
	}

	float TextBox::GetCaretLocalX(Renderer& renderer, const Theme& theme, std::wstring_view text, std::size_t caretIndex, tinycore::Rect textRect) const {
		float layoutWidth = GetTextLayoutWidth(renderer, theme, text, textRect);
		tinycore::Vec2 caretPosition = renderer.GetTextCaretPosition(text, caretIndex, { layoutWidth, textRect.h }, theme.textBox.fontSize, TextWrap::NoWrap);

		return caretPosition.x;
	}

	void TextBox::EnsureCaretVisible(Renderer& renderer, const Theme& theme, std::wstring_view text, std::size_t caretIndex, tinycore::Rect textRect) {
		if (textRect.w <= 0.0f || text.empty()) {
			m_scrollX = 0.0f;
			return;
		}

		float caretLocalX = GetCaretLocalX(renderer, theme, text, caretIndex, textRect);

		float visibleLeft = m_scrollX;
		float visibleRight = m_scrollX + textRect.w;
		if (caretLocalX < visibleLeft + m_scrollPadding)
			m_scrollX = caretLocalX - m_scrollPadding;
		else if (caretLocalX > visibleRight - m_scrollPadding)
			m_scrollX = caretLocalX - textRect.w + m_scrollPadding;

		tinycore::Size textSize = renderer.MeasureText(text, theme.textBox.fontSize, TextWrap::NoWrap);
		float maxScrollX = textSize.width - textRect.w + m_scrollPadding;
		if (maxScrollX < 0.0f)
			maxScrollX = 0.0f;

		if (m_scrollX < 0.0f)
			m_scrollX = 0.0f;

		if (m_scrollX > maxScrollX)
			m_scrollX = maxScrollX;
	}

	tinycore::Rect TextBox::GetScrolledTextRect(Renderer& renderer, const Theme& theme, std::wstring_view text, tinycore::Rect textRect) const {
		float layoutWidth = GetTextLayoutWidth(renderer, theme, text, textRect);

		return { textRect.x - m_scrollX, textRect.y, layoutWidth, textRect.h };
	}

	tinycore::Vec2 TextBox::GetTextLocalPosition(tinycore::Vec2 position, tinycore::Rect textRect) const {
		return { position.x - textRect.x + m_scrollX, position.y - textRect.y };
	}
}