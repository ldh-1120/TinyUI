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

	tinycore::Size StackLayout::Measure(Widget& parent, tinycore::Size availableSize) {
		if (m_direction == LayoutDirection::Vertical)
			return MeasureVertical(parent, availableSize);

		return MeasureHorizontal(parent, availableSize);
	}

	void StackLayout::Arrange(Widget& parent) {
		if (m_direction == LayoutDirection::Horizontal) {
			ArrangeHorizontal(parent);
			return;
		}

		ArrangeVertical(parent);
	}

	tinycore::Size StackLayout::MeasureHorizontal(Widget& parent, tinycore::Size availableSize) {
		tinycore::Size result { };

		std::size_t visibleCount = 0;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);

			if (!child || !child->IsVisible())
				continue;

			LayoutStyle& style = child->GetLayoutStyle();

			tinycore::Size childAvailableSize { availableSize.width, availableSize.height - style.margin.top - style.margin.bottom };
			tinycore::Size childSize = child->MeasureTree(childAvailableSize);

			float totalChildWidth = childSize.width + style.margin.left + style.margin.right;
			float totalChildHeight = childSize.height + style.margin.top + style.margin.bottom;

			result.width += totalChildWidth;
			if (totalChildHeight > result.height)
				result.height = totalChildHeight;

			++visibleCount;
		}

		if (visibleCount > 1)
			result.width += m_gap * static_cast<float>(visibleCount - 1);

		result.width += m_padding.left + m_padding.right;
		result.height += m_padding.top + m_padding.bottom;

		return result;
	}

	tinycore::Size StackLayout::MeasureVertical(Widget& parent, tinycore::Size availableSize) {
		tinycore::Size result { };

		std::size_t visibleCount = 0;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (!child || !child->IsVisible())
				continue;

			LayoutStyle& style = child->GetLayoutStyle();

			tinycore::Size childAvailableSize { availableSize.width - style.margin.left - style.margin.right, availableSize.height };
			tinycore::Size childSize = child->MeasureTree(childAvailableSize);

			float totalChildWidth = childSize.width + style.margin.left + style.margin.right;
			float totalChildHeight = childSize.height + style.margin.top + style.margin.bottom;
			if (totalChildWidth > result.width)
				result.width = totalChildWidth;

			result.height += totalChildHeight;

			++visibleCount;
		}

		if (visibleCount > 1)
			result.height += m_gap * static_cast<float>(visibleCount - 1);

		result.width += m_padding.left + m_padding.right;
		result.height += m_padding.top + m_padding.bottom;

		return result;
	}

	void StackLayout::ArrangeHorizontal(Widget& parent) {
		tinycore::Rect contentRect = parent.GetContentRect();

		float x = contentRect.x + m_padding.left;
		float y = contentRect.y + m_padding.top;

		float availableWidth = contentRect.w - m_padding.left - m_padding.right;
		float availableHeight = contentRect.h - m_padding.top - m_padding.bottom;

		if (availableWidth < 0.0f)
			availableWidth = 0.0f;

		if (availableHeight < 0.0f)
			availableHeight = 0.0f;

		std::size_t visibleCount = 0;
		float fixedWidth = 0.0f;
		float totalStretch = 0.0f;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (!child || !child->IsVisible())
				continue;

			LayoutStyle& style = child->GetLayoutStyle();

			++visibleCount;

			fixedWidth += style.margin.left + style.margin.right;
			if (style.stretch > 0.0f) {
				totalStretch += style.stretch;
				fixedWidth += style.minSize.width;
				continue;
			}

			tinycore::Size desiredSize = child->GetDesiredSize();

			float childWidth = desiredSize.width;
			if (style.preferredSize.width > 0.0f)
				childWidth = style.preferredSize.width;

			childWidth = tinycore::Clamp(childWidth, style.minSize.width, style.maxSize.width);

			fixedWidth += childWidth;
		}

		if (visibleCount > 1)
			fixedWidth += m_gap * static_cast<float>(visibleCount - 1);

		float remainingWidth = availableWidth - fixedWidth;

		if (remainingWidth < 0.0f)
			remainingWidth = 0.0f;

		std::size_t arrangedCount = 0;

		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);

			if (!child || !child->IsVisible())
				continue;

			LayoutStyle& style = child->GetLayoutStyle();
			tinycore::Size desiredSize = child->GetDesiredSize();

			float childWidth = desiredSize.width;
			float childHeight = desiredSize.height;
			if (style.stretch > 0.0f && totalStretch > 0.0f)
				childWidth = style.minSize.width + remainingWidth * (style.stretch / totalStretch);
			else if (style.preferredSize.width > 0.0f)
				childWidth = style.preferredSize.width;

			if (style.preferredSize.height > 0.0f)
				childHeight = style.preferredSize.height;

			float availableChildHeight = availableHeight - style.margin.top - style.margin.bottom;
			if (availableChildHeight < 0.0f)
				availableChildHeight = 0.0f;

			if (style.verticalAlignment == LayoutAlignment::Stretch)
				childHeight = availableChildHeight;

			childWidth = tinycore::Clamp(childWidth, style.minSize.width, style.maxSize.width);
			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			float childX = x + style.margin.left;
			float childY = y + style.margin.top;
			if (style.verticalAlignment == LayoutAlignment::Center)
				childY = y + style.margin.top + (availableChildHeight - childHeight) * 0.5f;
			else if (style.verticalAlignment == LayoutAlignment::End)
				childY = y + style.margin.top + availableChildHeight - childHeight;

			child->SetRect({ childX, childY, childWidth, childHeight });

			x += style.margin.left + childWidth + style.margin.right;

			++arrangedCount;
			if (arrangedCount < visibleCount)
				x += m_gap;
		}
	}

	void StackLayout::ArrangeVertical(Widget& parent) {
		tinycore::Rect contentRect = parent.GetContentRect();

		float x = contentRect.x + m_padding.left;
		float y = contentRect.y + m_padding.top;

		float availableWidth = contentRect.w - m_padding.left - m_padding.right;
		float availableHeight = contentRect.h - m_padding.top - m_padding.bottom;

		if (availableWidth < 0.0f)
			availableWidth = 0.0f;

		if (availableHeight < 0.0f)
			availableHeight = 0.0f;

		std::size_t visibleCount = 0;
		float fixedHeight = 0.0f;
		float totalStretch = 0.0f;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (!child || !child->IsVisible())
				continue;

			LayoutStyle& style = child->GetLayoutStyle();

			++visibleCount;

			fixedHeight += style.margin.top + style.margin.bottom;
			if (style.stretch > 0.0f) {
				totalStretch += style.stretch;
				fixedHeight += style.minSize.height;
				continue;
			}

			tinycore::Size desiredSize = child->GetDesiredSize();

			float childHeight = desiredSize.height;
			if (style.preferredSize.height > 0.0f)
				childHeight = style.preferredSize.height;

			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			fixedHeight += childHeight;
		}

		if (visibleCount > 1)
			fixedHeight += m_gap * static_cast<float>(visibleCount - 1);

		float remainingHeight = availableHeight - fixedHeight;
		if (remainingHeight < 0.0f)
			remainingHeight = 0.0f;

		std::size_t arrangedCount = 0;
		for (std::size_t index = 0; index < parent.GetChildCount(); ++index) {
			Widget* child = parent.GetChild(index);
			if (!child || !child->IsVisible())
				continue;

			LayoutStyle& style = child->GetLayoutStyle();
			tinycore::Size desiredSize = child->GetDesiredSize();

			float childWidth = desiredSize.width;
			float childHeight = desiredSize.height;
			if (style.stretch > 0.0f && totalStretch > 0.0f)
				childHeight = style.minSize.height + remainingHeight * (style.stretch / totalStretch);
			else if (style.preferredSize.height > 0.0f)
				childHeight = style.preferredSize.height;

			float availableChildWidth = availableWidth - style.margin.left - style.margin.right;
			if (availableChildWidth < 0.0f)
				availableChildWidth = 0.0f;

			if (style.horizontalAlignment == LayoutAlignment::Stretch)
				childWidth = availableChildWidth;

			if (style.preferredSize.width > 0.0f)
				childWidth = style.preferredSize.width;

			childWidth = tinycore::Clamp(childWidth, style.minSize.width, style.maxSize.width);
			childHeight = tinycore::Clamp(childHeight, style.minSize.height, style.maxSize.height);

			float childX = x + style.margin.left;
			float childY = y + style.margin.top;

			if (style.horizontalAlignment == LayoutAlignment::Center)
				childX = x + style.margin.left + (availableChildWidth - childWidth) * 0.5f;
			else if (style.horizontalAlignment == LayoutAlignment::End)
				childX = x + style.margin.left + availableChildWidth - childWidth;

			child->SetRect({ childX, childY, childWidth, childHeight });

			y += style.margin.top + childHeight + style.margin.bottom;

			++arrangedCount;
			if (arrangedCount < visibleCount)
				y += m_gap;
		}
	}
}