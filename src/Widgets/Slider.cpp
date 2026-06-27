#include "pch.h"

#include <TinyUI/Widgets/Slider.h>

#include <TinyCore/Math/Numeric.h>
#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Theme/Theme.h>

namespace tinyui {
	Slider::Slider() {
		GetLayoutStyle().preferredSize = { 160.f, 28.f };
		GetLayoutStyle().verticalAlignment = LayoutAlignment::Center;
	}

	void Slider::SetValue(float value) {
		float clampedValue = tinycore::Clamp(value, m_minimum, m_maximum);
		if (m_value == clampedValue)
			return;

		m_value = clampedValue;
		m_changed = true;
	}

	float Slider::GetValue() const {
		return m_value;
	}

	void Slider::SetRange(float minimum, float maximum) {
		m_minimum = minimum;
		m_maximum = maximum;
		if (m_minimum >= m_maximum)
			m_minimum = m_maximum;
		
		SetValue(m_value);
	}

	float Slider::GetMinimum() const {
		return m_minimum;
	}

	float Slider::GetMaximum() const {
		return m_maximum;
	}

	void Slider::SetOptions(const SliderOptions& options) {
		m_options = options;
	}

	const SliderOptions& Slider::GetOptions() const {
		return m_options;
	}

	bool Slider::IsDragging() const {
		return m_dragging;
	}

	bool Slider::TakeChanged() {
		bool changed = m_changed;
		m_changed = false;

		return changed;
	}

	bool Slider::IsMouseInteractive() const {
		return true;
	}

	bool Slider::IsFocusable() const {
		return true;
	}

	void Slider::OnPaint(PaintContext& context) {
		const Theme& theme = context.theme;
		tinycore::Rect rect = GetRect();

		float trackHeight = GetTrackHeight(theme);
		float thumbRadius = GetThumbRadius(theme);
		tinycore::Rect trackRect { rect.x + thumbRadius, rect.y + (rect.h - trackHeight) * 0.5f, rect.w - thumbRadius * 2.f, trackHeight };

		float normalizedValue = GetNormalizedValue();
		float thumbX = trackRect.x + trackRect.w * normalizedValue;
		float thumbY = rect.y + rect.h * 0.5f;
		tinycore::Rect fillRect { trackRect.x, trackRect.y, thumbX - trackRect.x, trackRect.h };

		tinycore::Color thumbColor = theme.slider.thumb;
		if (m_dragging)
			thumbColor = theme.slider.thumbPressed;
		else if (IsHovered())
			thumbColor = theme.slider.thumbHovered;

		context.renderer.FillRect(trackRect, theme.slider.track, trackHeight * 0.5f);
		context.renderer.FillRect(fillRect, theme.slider.fill, trackHeight * 0.5f);
		context.renderer.FillCircle({ thumbX, thumbY }, thumbRadius, thumbColor);
	}

	void Slider::OnMouseDown(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left)
			return;

		m_dragging = true;
		SetValueFromPosition(event.position);
		event.Accept();
	}

	void Slider::OnMouseMove(MouseEvent& event) {
		if (!m_dragging)
			return;

		SetValueFromPosition(event.position);
		event.Accept();
	}

	void Slider::OnMouseUp(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left || !m_dragging)
			return;

		m_dragging = false;
		SetValueFromPosition(event.position);
		event.Accept();
	}

	void Slider::SetValueFromPosition(tinycore::Vec2 position) {
		tinycore::Rect rect = GetRect();

		float thumbRadius = 7.f;
		if (m_options.thumbRadius >= 0.f)
			thumbRadius = m_options.thumbRadius;

		float trackX = rect.x + thumbRadius;
		float trackWidth = rect.w - thumbRadius * 2.f;
		if (trackWidth <= 0.f)
			return;

		float normalizedValue = (position.x - trackX) / trackWidth;
		normalizedValue = tinycore::Clamp01(normalizedValue);

		float value = m_minimum + (m_maximum - m_minimum) * normalizedValue;
		SetValue(value);
	}

	float Slider::GetNormalizedValue() const {
		float range = m_maximum - m_minimum;
		if (range <= 0.f)
			return 0.f;

		return tinycore::Clamp01((m_value - m_minimum) / range);
	}

	float Slider::GetTrackHeight(const Theme& theme) const {
		if (m_options.thumbRadius >= 0.f)
			return m_options.thumbRadius;

		return theme.slider.thumbRadius;
	}

	float Slider::GetThumbRadius(const Theme& theme) const {
		if (m_options.thumbRadius >= 0.f)
			return m_options.thumbRadius;

		return theme.slider.thumbRadius;
	}
}