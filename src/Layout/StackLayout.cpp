#include "pch.h"

#include <TinyCore/Math/Numeric.h>

#include <TinyUI/Layout/StackLayout.h>
#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
	static float AlignOffset(float availableSize, float childSize, tinyui::LayoutAlignment alignment) {
		if (alignment == tinyui::LayoutAlignment::Center)
			return (availableSize - childSize) * 0.5f;

		if (alignment == tinyui::LayoutAlignment::End)
			return availableSize - childSize;

		return 0.f;
	}

	StackLayout::StackLayout(LayoutDirection direction) : m_direction(direction) { }

	void StackLayout::SetDirection(LayoutDirection direction) {
		m_direction = direction;
	}

	LayoutDirection StackLayout::GetDirection() const {
		return m_direction;
	}

	void StackLayout::SetPadding(Thickness padding) {
		m_padding = padding;
	}

	Thickness StackLayout::GetPadding() const {
		return m_padding;
	}

	void StackLayout::SetGap(float gap) {
		m_gap = gap;
	}

	float StackLayout::GetGap() const {
		return m_gap;
	}

	void StackLayout::Arrange(Widget& parent) {
		if (m_direction == LayoutDirection::Horizontal) {
			ArrangeHorizontal(parent);
			return;
		}

		ArrangeVertical(parent);
	}

	void StackLayout::ArrangeHorizontal(Widget& parent) {
		Rect parentRect = parent.GetContentRect();

		std::size_t visibleChildCount = 0;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (child && child->IsVisible())
				++visibleChildCount;
		}

		if (visibleChildCount == 0)
			return;

		float totalGap = m_gap * static_cast<float>(visibleChildCount - 1);

		float availableWidth = parentRect.w - m_padding.left - m_padding.right - totalGap;
		if (availableWidth < 0.f)
			availableWidth = 0.f;

		float availableHeight = parentRect.h - m_padding.top - m_padding.bottom;
		if (availableHeight < 0.f)
			availableHeight = 0.f;

		float fixedWidth = 0.f;
		float totalStretch = 0.f;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (!child || !child->IsVisible())
				continue;

			const LayoutStyle& style = child->GetLayoutStyle();
			fixedWidth += style.margin.left, style.margin.right;
			if (style.stretch > 0.f) {
				totalStretch += style.stretch;
				continue;
			}

			float childWidth = style.preferredSize.width;
			childWidth = tinycore::Clamp(childWidth, style.minSize.width, style.maxSize.width);

			fixedWidth += childWidth;
		}

		float remainingWidth = availableWidth - fixedWidth;
		if (remainingWidth < 0.f)
			remainingWidth = 0.f;

		float currentX = parentRect.x + m_padding.left;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (!child || !child->IsVisible())
				continue;

			const LayoutStyle& style = child->GetLayoutStyle();
			currentX += style.margin.left;
			
			float childWidth = style.preferredSize.width;
			if (style.stretch > 0.f && totalStretch > 0.f)
				childWidth = remainingWidth * (style.stretch / totalStretch);

			childWidth = tinycore::Clamp(childWidth, style.minSize.width, style.maxSize.width);

			float availableChildHeight = availableHeight - style.margin.top - style.margin.bottom;
			if (availableChildHeight < 0.0f)
				availableChildHeight = 0.0f;

			float childHeight = availableChildHeight;
			if (style.verticalAlignment != LayoutAlignment::Stretch && style.preferredSize.height > 0.0f)
				childHeight = style.preferredSize.height;

			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			float childY = parentRect.y + m_padding.top + style.margin.top;
			if (style.verticalAlignment != LayoutAlignment::Stretch)
				childY += AlignOffset(availableChildHeight, childHeight, style.verticalAlignment);

			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			child->SetRect({ currentX, childY, childWidth, childHeight });

			currentX += childWidth + style.margin.right + m_gap;
		}
	}

	void StackLayout::ArrangeVertical(Widget& parent) {
		tinycore::Rect parentRect = parent.GetContentRect();

		std::size_t visibleChildCount = 0;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);

			if (child && child->IsVisible())
				++visibleChildCount;
		}

		if (visibleChildCount == 0)
			return;

		float totalGap = m_gap * static_cast<float>(visibleChildCount - 1);

		float availableWidth = parentRect.w - m_padding.left - m_padding.right;
		float availableHeight = parentRect.h - m_padding.top - m_padding.bottom - totalGap;
		if (availableWidth < 0.0f)
			availableWidth = 0.0f;

		if (availableHeight < 0.0f)
			availableHeight = 0.0f;

		float fixedHeight = 0.0f;
		float totalStretch = 0.0f;

		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);

			if (!child || !child->IsVisible())
				continue;

			const LayoutStyle& style = child->GetLayoutStyle();
			fixedHeight += style.margin.top + style.margin.bottom;
			if (style.stretch > 0.0f) {
				totalStretch += style.stretch;
				continue;
			}

			float childHeight = style.preferredSize.height;
			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			fixedHeight += childHeight;
		}

		float remainingHeight = availableHeight - fixedHeight;
		if (remainingHeight < 0.0f)
			remainingHeight = 0.0f;

		float currentY = parentRect.y + m_padding.top;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);

			if (!child || !child->IsVisible())
				continue;

			const LayoutStyle& style = child->GetLayoutStyle();

			currentY += style.margin.top;

			float childHeight = style.preferredSize.height;
			if (style.stretch > 0.0f && totalStretch > 0.0f)
				childHeight = remainingHeight * (style.stretch / totalStretch);

			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			float availableChildWidth = availableWidth - style.margin.left - style.margin.right;
			if (availableChildWidth < 0.f)
				availableChildWidth = 0.f;

			float childWidth = availableChildWidth;
			if (style.horizontalAlignment != LayoutAlignment::Stretch && style.preferredSize.width > 0.f)
				childWidth = style.preferredSize.width;

			childWidth = tinycore::Clamp(childWidth, style.minSize.width, style.maxSize.width);
			float childX = parentRect.x + m_padding.left + style.margin.left;
			if (style.horizontalAlignment != LayoutAlignment::Stretch)
				childX += AlignOffset(availableChildWidth, childWidth, style.horizontalAlignment);

			child->SetRect({ childX, currentY, childWidth, childHeight });

			currentY += childHeight + style.margin.bottom + m_gap;
		}
	}
}