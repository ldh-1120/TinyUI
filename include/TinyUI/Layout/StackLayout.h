#pragma once

#include <TinyUI/Layout/Layout.h>
#include <TinyUI/Layout/LayoutTypes.h>

#include <TinyCore/Core/Types.h>

using namespace tinycore;

namespace tinyui {
	class StackLayout final : public Layout {
	public:
		explicit StackLayout(LayoutDirection direction);

		void SetDirection(LayoutDirection direction);
		LayoutDirection GetDirection() const;

		void SetPadding(Thickness padding);
		Thickness GetPadding() const;

		void SetGap(float gap);
		float GetGap() const;

		tinycore::Size Measure(Widget& parent, tinycore::Size availableSize) override;
		void Arrange(Widget& parent) override;

	private:
		tinycore::Size MeasureVertical(Widget& parent, tinycore::Size availableSize);
		tinycore::Size MeasureHorizontal(Widget& parent, tinycore::Size availableSize);
		void ArrangeHorizontal(Widget& parent);
		void ArrangeVertical(Widget& parent);

	private:
		LayoutDirection m_direction = LayoutDirection::Vertical;
		Thickness m_padding { };
		float m_gap = 0.f;
	};
}