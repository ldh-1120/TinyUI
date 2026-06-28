#pragma once

#include <TinyUI/Events/KeyEvent.h>
#include <TinyUI/Events/MouseEvent.h>
#include <TinyUI/Events/TextInputEvent.h>
#include <TinyUI/Widgets/Widget.h>

#include <string>
#include <string_view>

namespace tinyui {
	class Renderer;
	struct Theme;

	struct TextBoxOptions {
		std::wstring placeholder { };

		bool readOnly = false;
	};

	struct TextBoxResult {
		std::wstring text { };
		bool changed = false;

		class TextBox* widget = nullptr;
		class TextBox& GetWidget() const {
			return *widget;
		}
	};

	class TextBox final : public Widget {
	public:
		TextBox();

		void SetText(std::wstring_view text);
		const std::wstring& GetText() const;

		void SetOptions(const TextBoxOptions& options);
		const TextBoxOptions& GetOptions() const;

		bool HasPendingChange() const;
		bool TakeChanged();

		bool HasSelection() const;
		std::size_t GetSelectionStart() const;
		std::size_t GetSelectionEnd() const;

		bool IsMouseInteractive() const override;
		bool IsFocusable() const override;

		bool ShouldDrawFocusRing() const override;

		bool GetImeCandidatePosition(tinycore::Vec2& position) const override;

	protected:
		void OnPaint(PaintContext& context) override;
		bool OnUpdate(float deltaTime) override;
		void OnBlur() override;

		void OnMouseDown(MouseEvent& event) override;
		void OnMouseMove(MouseEvent& event) override;
		void OnMouseUp(MouseEvent& event) override;

		void OnKeyDown(KeyEvent& event) override;
		void OnTextInput(TextInputEvent& event) override;
		void OnTextComposition(TextCompositionEvent& event) override;

	private:
		void SetTextInternal(std::wstring_view text, bool notifyChanged);

		void InsertCharacter(wchar_t character);
		void Backspace();
		void Delete();

		void MoveCaretLeft(bool keepSelection);
		void MoveCaretRight(bool keepSelection);
		void MoveCaretHome(bool keepSelection);
		void MoveCaretEnd(bool keepSelection);

		bool IsEditableCharacter(wchar_t character) const;

		tinycore::Rect GetTextRect(const Theme& theme) const;
		float GetCaretX(PaintContext& context, tinycore::Rect textRect) const;

		void MoveCaretToPoint(PaintContext& context, tinycore::Vec2 position);

		void SetCaretIndex(std::size_t index, bool keepSelection);
		void ClearSelection();
		bool DeleteSelection();

		std::size_t HitTestCaretIndex(Renderer& renderer, const Theme& theme, tinycore::Vec2 position) const;

		void PaintSelection(PaintContext& context, tinycore::Rect textRect);

		bool IsWordCharacter(wchar_t character) const;

		std::size_t FindPreviousWordBoundary(std::size_t index) const;
		std::size_t FindNextWordBoundary(std::size_t index) const;

		void MoveCaretPreviousWord(bool keepSelection);
		void MoveCaretNextWord(bool keepSelection);

		void DeletePreviousWord();
		void DeleteNextWord();

		void SelectAll();

		std::wstring GetSelectedText() const;

		void CopySelectionToClipboard();
		void CutSelectionToClipboard();
		void PasteFromClipboard();

		std::wstring GetDisplayText() const;
		std::size_t GetDisplayCaretIndex() const;

		void PaintCompositionUnderline(PaintContext& context, tinycore::Rect textRect);

		bool CommitCompositionText();

		float GetTextLayoutWidth(Renderer& renderer, const Theme& theme, std::wstring_view text, tinycore::Rect textRect) const;
		float GetCaretLocalX(Renderer& renderer, const Theme& theme, std::wstring_view text, std::size_t caretIndex, tinycore::Rect textRect) const;
		void EnsureCaretVisible(Renderer& renderer, const Theme& theme, std::wstring_view text, std::size_t caretIndex, tinycore::Rect textRect);

		tinycore::Rect GetScrolledTextRect(Renderer& renderer, const Theme& theme, std::wstring_view text, tinycore::Rect textRect) const;
		tinycore::Vec2 GetTextLocalPosition(tinycore::Vec2 position, tinycore::Rect textRect) const;

	private:
		std::wstring m_text { };
		TextBoxOptions m_options { };

		std::size_t m_caretIndex = 0;

		bool m_changed = false;

		float m_hoverT = 0.f;
		float m_focusT = 0.f;
		float m_caretBlinkTimer = 0.f;

		std::size_t m_selectionAnchorIndex = 0;
		bool m_selecting = false;

		std::wstring m_compositionText { };

		tinycore::Vec2 m_imeCandidatePosition { };
		bool m_hasImeCandidatePosition = false;

		float m_scrollX = 0.f;
		float m_scrollPadding = 0.f;
	};
}