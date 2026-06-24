#include <TinyUI/Widgets/Button.h>

#include <utility>

namespace tinyui {
	Button::Button(std::wstring text) {
		GetLayoutStyle().preferredSize = { 96.f, 36.f };
	}

	void Button::SetText(std::wstring_view text) {
		m_text = text;
	}

	const std::wstring& Button::GetText() const {
		return m_text;
	}

	void Button::SetOptions(const ButtonOptions& options) {
		m_options = options;
	}

	const ButtonOptions& Button::GetOptions() const {
		return m_options;
	}

	bool Button::IsDown() const {
		return m_down;
	}

	bool Button::TakeClicked() {
		bool clicked = m_clicked;

		m_clicked = false;

		return clicked;
	}

	void Button::OnPaint(PaintContext& context) {
		Renderer& renderer = context.renderer;
		const Theme& theme = context.theme;

		tinycore::Rect rect = GetRect();
		tinycore::Color backgroundColor = theme.button.normal;
		if (!IsEnabled())
			backgroundColor = theme.button.disabled;
		else if (m_down)
			backgroundColor = theme.button.pressed;
		else if (IsHovered())
			backgroundColor = theme.button.hovered;

		tinycore::Color textColor = IsEnabled() ? theme.button.text : theme.button.disabledText;
		float cornerRadius = m_options.cornerRadius >= 0.f ? m_options.cornerRadius : theme.button.cornerRadius;
		float borderThickness = m_options.borderThickness >= 0.f ? m_options.borderThickness : theme.button.borderThickness;
		float fontSize = m_options.fontSize >= 0.f ? m_options.fontSize : theme.button.fontSize;
		renderer.FillRect(rect, backgroundColor, cornerRadius);
		if (borderThickness > 0.f)
			renderer.DrawRect(rect, theme.button.border, borderThickness, cornerRadius);

		renderer.DrawTextBox(m_text, rect, textColor, fontSize, TextAlign::Center, TextWrap::NoWrap);
	}

	void Button::OnMouseDown(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left)
			return;

		m_down = true;

		event.Accept();
	}

	void Button::OnMouseUp(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left)
			return;

		bool wasDown = m_down;

		m_down = false;

		if (wasDown && GetRect().Contains(event.position))
			m_clicked = true;

		event.Accept();
	}
}