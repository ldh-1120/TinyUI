#include "pch.h"

#include <TinyCore/Math/Numeric.h>
#include <TinyUI/Widgets/Button.h>
#include <TinyUI/Rendering/IconPainter.h>

#include <utility>

namespace tinyui {
	Button::Button(std::wstring text) {
		GetLayoutStyle().preferredSize = { 96.f, 36.f };
		GetLayoutStyle().verticalAlignment = LayoutAlignment::Center;
	}

	void Button::SetIcon(ButtonIcon icon) {
		m_icon = icon;
	}

	ButtonIcon Button::GetIcon() const {
		return m_icon;
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

	bool Button::IsMouseInteractive() const {
		return true;
	}

	void Button::OnPaint(PaintContext& context) {
		Rect rect = GetRect();

		ButtonStyle style = ResolveStyle(context.theme);
		ButtonStateStyle stateStyle = BlendStateStyle(style);
		context.renderer.FillRect(rect, stateStyle.background, style.cornerRadius);
		if (style.borderThickness > 0.f)
			context.renderer.DrawRect(rect, stateStyle.border, style.borderThickness, style.cornerRadius);

		if (m_icon != ButtonIcon::None) {
			IconPainter::DrawButtonIcon(context.renderer, m_icon, rect, stateStyle.text, style.iconSize, style.iconThickness);
			return;
		}
		context.renderer.DrawTextBox(m_text, rect, stateStyle.text, style.fontSize, TextAlign::Center, TextWrap::NoWrap);
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

	bool Button::OnUpdate(float deltaTime) {
		float previousHoverT = m_hoverT;
		float previousPressedT = m_pressedT;

		float hoverTarget = IsHovered() ? 1.f : 0.f;
		float pressedTarget = IsDown() ? 1.f : 0.f;

		const float hoverSpeed = 10.f;
		const float pressedSpeed = 14.f;

		m_hoverT = tinycore::MoveTowards(m_hoverT, hoverTarget, hoverSpeed * deltaTime);
		m_pressedT = tinycore::MoveTowards(m_pressedT, pressedTarget, pressedSpeed * deltaTime);

		return previousHoverT != m_hoverT || previousPressedT != m_pressedT;
	}

	ButtonStyle Button::ResolveStyle(const Theme& theme) const {
		if (m_options.useCustomStyle)
			return m_options.customStyle;

		if (m_options.variant == ButtonVariant::Primary)
			return theme.button.primary;

		if (m_options.variant == ButtonVariant::Ghost)
			return theme.button.ghost;

		if (m_options.variant == ButtonVariant::TitleBar)
			return theme.button.titleBar;

		if (m_options.variant == ButtonVariant::TitleBarClose)
			return theme.button.titleBarClose;

		return theme.button.secondary;
	}

	ButtonStateStyle Button::ResolveStateStyle(const ButtonStyle& style) const {
		if (!IsEnabled())
			return style.disabled;

		if (m_down)
			return style.pressed;

		if (IsHovered())
			return style.hovered;

		return style.normal;
	}

	ButtonStateStyle Button::BlendStateStyle(const ButtonStyle& style) const {
		if (!IsEnabled())
			return style.disabled;

		ButtonStateStyle result { };
		ButtonStateStyle hoverStyle { };
		hoverStyle.background = tinycore::LerpColor(style.normal.background, style.hovered.background, m_hoverT);
		hoverStyle.border = tinycore::LerpColor(style.normal.border, style.hovered.border, m_hoverT);
		hoverStyle.text = tinycore::LerpColor(style.normal.text, style.hovered.text, m_hoverT);

		result.background = tinycore::LerpColor(hoverStyle.background, style.pressed.background, m_pressedT);
		result.border = tinycore::LerpColor(hoverStyle.border, style.pressed.border, m_pressedT);
		result.text = tinycore::LerpColor(hoverStyle.text, style.pressed.text, m_pressedT);

		return result;
	}
}