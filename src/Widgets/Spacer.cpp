#include "pch.h"

#include <TinyUI/Widgets/Spacer.h>

namespace tinyui {
	Spacer::Spacer() {
		GetLayoutStyle().preferredSize = { 0.f, 0.f };
		GetLayoutStyle().stretch = 1.f;
	}

	void Spacer::SetPreferredSize(tinycore::Size size) {
		GetLayoutStyle().preferredSize = size;
	}

	void Spacer::SetStretch(float stretch) {
		GetLayoutStyle().stretch = stretch;
	}

	void Spacer::OnPaint(PaintContext& context) { }
}