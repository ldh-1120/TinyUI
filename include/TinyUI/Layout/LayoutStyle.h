#pragma once

#include <TinyCore/Core/Types.h>

#include <TinyUI/Layout/LayoutTypes.h>

using namespace tinycore;

namespace tinyui {
	struct LayoutStyle {
		Size preferredSize { };
		Size minSize { };
		Size maxSize { 100000.f, 100000.f };

		Thickness margin { };

		float stretch = 0.f;

		LayoutAlignment horizontalAlignment = LayoutAlignment::Stretch;
		LayoutAlignment verticalAlignment = LayoutAlignment::Stretch;

		LayoutStyle& Size(tinycore::Size size) {
			preferredSize = size;
			return *this;
		}

        LayoutStyle& Width(float width) {
            preferredSize.width = width;
            return *this;
        }

        LayoutStyle& Height(float height) {
            preferredSize.height = height;
            return *this;
        }

        LayoutStyle& MinSize(tinycore::Size size) {
            minSize = size;
            return *this;
        }

        LayoutStyle& MaxSize(tinycore::Size size) {
            maxSize = size;
            return *this;
        }

        LayoutStyle& Margin(tinycore::Thickness value) {
            margin = value;
            return *this;
        }

        LayoutStyle& Stretch(float value = 1.0f) {
            stretch = value;
            return *this;
        }

        LayoutStyle& AlignX(LayoutAlignment alignment) {
            horizontalAlignment = alignment;
            return *this;
        }

        LayoutStyle& AlignY(LayoutAlignment alignment) {
            verticalAlignment = alignment;
            return *this;
        }

        LayoutStyle& Align(LayoutAlignment horizontal, LayoutAlignment vertical) {
            horizontalAlignment = horizontal;
            verticalAlignment = vertical;
            return *this;
        }
	};
}