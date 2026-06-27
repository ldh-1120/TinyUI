#pragma once

#include <TinyUI/Events/KeyEvent.h>
#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
	struct SliderOptions {
		float trackHeight = -1.f;
		float thumbRadius = -1.f;
	};

	class Slider final : public Widget {
	public:
		Slider();

		void SetValue(float value);
		float GetValue() const;

		void SetRange(float minimum, float maximum);
		float GetMinimum() const;
		float GetMaximum() const;

		void SetOptions(const SliderOptions& options);
		const SliderOptions& GetOptions() const;

		bool IsDragging() const;
		bool TakeChanged();

		bool IsMouseInteractive() const override;
		bool IsFocusable() const override;

	protected:
		void OnPaint(PaintContext& context);

		void OnMouseDown(MouseEvent& event) override;
		void OnMouseMove(MouseEvent& event) override;
		void OnMouseUp(MouseEvent& event) override;

	private:
		void SetValueFromPosition(tinycore::Vec2 position);

		float GetNormalizedValue() const;
		float GetTrackHeight(const Theme& theme) const;
		float GetThumbRadius(const Theme& theme) const;

	private:
		float m_value = 0.0f;
		float m_minimum = 0.0f;
		float m_maximum = 1.0f;

		bool m_dragging = false;
		bool m_changed = false;

		SliderOptions m_options { };
	};
}