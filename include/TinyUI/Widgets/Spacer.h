#pragma once

#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
	class Spacer final : public Widget {
	public:
		Spacer();

		void SetPreferredSize(tinycore::Size size);
		void SetStretch(float stretch);

	protected:
		void OnPaint(PaintContext& context);
	};
}