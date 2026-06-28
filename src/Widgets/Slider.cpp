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
		SetValueInternal(value, false);
	}

	float Slider::GetValue() const {
		return m_value;
	}

	void Slider::SetUserValue(float value) {
		SetValueInternal(value, true);
	}

	void Slider::SetValueInternal(float value, bool notifyChanged) {
		float clampedValue = tinycore::Clamp(value, m_minimum, m_maximum);
		if (m_value == clampedValue)
			return;

		m_value = clampedValue;
		if (notifyChanged)
			m_changed = true;
	}

	bool Slider::HasPendingChange() const {
		return m_changed;
	}

	void Slider::SetRange(float minimum, float maximum) {
		m_minimum = minimum;
		m_maximum = maximum;
		if (m_minimum >= m_maximum)
			m_minimum = m_maximum;
		
		SetValueInternal(m_value, false);
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
		float visualThumbRadius = GetVisualThumbRadius(theme);
		tinycore::Rect trackRect { rect.x + thumbRadius, rect.y + (rect.h - trackHeight) * 0.5f, rect.w - thumbRadius * 2.f, trackHeight };

		float normalizedValue = GetNormalizedValue();
		float thumbX = trackRect.x + trackRect.w * normalizedValue;
		float thumbY = rect.y + rect.h * 0.5f;
		tinycore::Rect fillRect { trackRect.x, trackRect.y, thumbX - trackRect.x, trackRect.h };

		tinycore::Color thumbColor = GetAnimatedThumbColor(theme);
		context.renderer.FillRect(trackRect, theme.slider.track, trackHeight * 0.5f);
		context.renderer.FillRect(fillRect, theme.slider.fill, trackHeight * 0.5f);
		context.renderer.FillCircle({ thumbX, thumbY }, visualThumbRadius, thumbColor);
	}

	void Slider::OnMouseDown(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left)
			return;

		m_dragging = true;
		SetValueFromPosition(event.position, *event.theme);
		event.Accept();
	}

	void Slider::OnMouseMove(MouseEvent& event) {
		if (!m_dragging)
			return;

		SetValueFromPosition(event.position, *event.theme);
		event.Accept();
	}

	void Slider::OnMouseUp(MouseEvent& event) {
		if (event.button != tinycore::MouseButton::Left || !m_dragging)
			return;

		m_dragging = false;
		SetValueFromPosition(event.position, *event.theme);
		event.Accept();
	}

	void Slider::OnKeyDown(KeyEvent& event) {
		if (event.key == tinycore::KeyCode::Left || event.key == tinycore::KeyCode::Down) {
			float step = event.shiftDown ? GetLargeStep() : GetSmallStep();
			AddUserValue(-step);

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::Right || event.key == tinycore::KeyCode::Up) {
			float step = event.shiftDown ? GetLargeStep() : GetSmallStep();
			AddUserValue(step);

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::PageDown) {
			AddUserValue(-GetLargeStep());

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::PageUp) {
			AddUserValue(GetLargeStep());

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::Home) {
			SetUserValue(m_minimum);

			event.Accept();
			return;
		}

		if (event.key == tinycore::KeyCode::End) {
			SetUserValue(m_maximum);

			event.Accept();
			return;
		}
	}

	bool Slider::OnUpdate(float deltaTime) {
		float previousHoverT = m_hoverT;
		float previousDragT = m_dragT;

		float hoverTarget = IsHovered() ? 1.f : 0.f;
		float dragTarget = m_dragging ? 1.f : 0.f;

		const float hoverSpeed = 12.f;
		const float dragSpeed = 12.f;

		m_hoverT = tinycore::MoveTowards(m_hoverT, hoverTarget, hoverSpeed * deltaTime);
		m_dragT = tinycore::MoveTowards(m_dragT, dragTarget, dragSpeed * deltaTime);

		return previousHoverT != m_hoverT || previousDragT != m_dragT;
	}

	void Slider::AddUserValue(float delta) {
		SetUserValue(m_value + delta);
	}

	float Slider::GetSmallStep() const {
		if (m_options.smallStep <= 0.0f)
			return 0.01f;

		return m_options.smallStep;
	}

	float Slider::GetLargeStep() const {
		if (m_options.largeStep <= 0.0f)
			return GetSmallStep() * 10.0f;

		return m_options.largeStep;
	}

	tinycore::Color Slider::GetAnimatedThumbColor(const Theme& theme) const {
		tinycore::Color hoverColor = tinycore::LerpColor(theme.slider.thumb, theme.slider.thumbHovered, m_hoverT);

		return tinycore::LerpColor(hoverColor, theme.slider.thumbPressed, m_dragT);
	}

	float Slider::GetVisualThumbRadius(const Theme& theme) const {
		float baseRadius = GetThumbRadius(theme);

		float hoverRadius = baseRadius * theme.slider.thumbHoverScale;
		float pressedRadius = baseRadius * theme.slider.thumbPressedScale;

		float radius = tinycore::Lerp(baseRadius, hoverRadius, m_hoverT);

		return tinycore::Lerp(radius, pressedRadius, m_dragT);
	}

	void Slider::SetValueFromPosition(tinycore::Vec2 position, const Theme& theme) {
		tinycore::Rect rect = GetRect();

		float thumbRadius = GetThumbRadius(theme);
		if (m_options.thumbRadius >= 0.f)
			thumbRadius = m_options.thumbRadius;

		float trackX = rect.x + thumbRadius;
		float trackWidth = rect.w - thumbRadius * 2.f;
		if (trackWidth <= 0.f)
			return;

		float normalizedValue = (position.x - trackX) / trackWidth;
		normalizedValue = tinycore::Clamp01(normalizedValue);

		float value = m_minimum + (m_maximum - m_minimum) * normalizedValue;
		SetUserValue(value);
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