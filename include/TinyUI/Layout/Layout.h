#pragma once

#include <TinyUI/TinyUI.h>

namespace tinyui {
	struct RowLayout {
		Rect bounds { };

		float currentX = 0.f;
		float gap = 8.f;

		RowLayout(Rect bounds, float gap = 8.f) : bounds(bounds), currentX(bounds.x), gap(gap) { };

		Rect Next(float width, float height = 0.f) {
			if (height <= 0.f)
				height = bounds.h;

			Rect result { currentX, bounds.y, width, height };
			currentX += width + gap;

			return result;
		}

		void Skip(float width) {
			currentX += width + gap;
		}

		Rect Remaining() const {
			float right = bounds.x + bounds.w;
			float remainingWidth = right - currentX;
			if (remainingWidth < 0.f)
				remainingWidth = 0.f;

			return Rect(currentX, bounds.y, remainingWidth, bounds.h);
		}
	};

	struct ColumnLayout {
		Rect bounds { };

		float currentY = 0.f;
		float gap = 8.f;

		ColumnLayout(Rect bounds, float gap = 8.f) : bounds(bounds), currentY(bounds.y), gap(gap) {};

		Rect Next(float height, float width = 0.f) {
			if (width <= 0.f)
				width = bounds.w;

			Rect result { bounds.x, currentY, width, height };
			currentY += height + gap;

			return result;
		}

		void Skip(float height) {
			currentY += height + gap;
		}

		Rect Remaining() const {
			float bottom = bounds.y + bounds.h;
			float remainingHeight = bottom - currentY;
			if (remainingHeight < 0.f)
				remainingHeight = 0.f;

			return Rect { bounds.x, currentY, bounds.w, remainingHeight };
		}
	};

	inline RowLayout Row(Rect bounds, float gap = 8.f) {
		return RowLayout(bounds, gap);
	}

	inline ColumnLayout Column(Rect bounds, float gap = 8.f) {
		return ColumnLayout(bounds, gap);
	}
}