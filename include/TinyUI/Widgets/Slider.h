#pragma once

#include <TinyUI/Events/KeyEvent.h>
#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
	struct SliderOptions {
		float trackHeight = -1.f;
		float thumbRadius = -1.f;

		float smallStep = 0.01f;
		float largeStep = 0.1f;
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

		bool HasPendingChange() const;

	protected:
		void OnPaint(PaintContext& context);

		void OnMouseDown(MouseEvent& event) override;
		void OnMouseMove(MouseEvent& event) override;
		void OnMouseUp(MouseEvent& event) override;

		void OnKeyDown(KeyEvent& event) override;

		bool OnUpdate(float deltaTime) override;

	private:
		void SetValueFromPosition(tinycore::Vec2 position, const Theme& theme);

		float GetNormalizedValue() const;
		float GetTrackHeight(const Theme& theme) const;
		float GetThumbRadius(const Theme& theme) const;

		void SetValueInternal(float value, bool notifyChanged);
		void SetUserValue(float value);	

		void AddUserValue(float delta);
		float GetSmallStep() const;
		float GetLargeStep() const;

		tinycore::Color GetAnimatedThumbColor(const Theme& theme) const;
		float GetVisualThumbRadius(const Theme& theme) const;

	private:
		float m_value = 0.0f;
		float m_minimum = 0.0f;
		float m_maximum = 1.0f;

		bool m_dragging = false;
		bool m_changed = false;

		float m_hoverT = 0.0f;
		float m_dragT = 0.0f;

		SliderOptions m_options { };
	};
}