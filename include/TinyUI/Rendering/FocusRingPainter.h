#pragma once

#include <TinyCore/Core/Types.h>

#include <TinyUI/Widgets/Style/FocusRingStyle.h>

namespace tinyui {
	class Renderer;

	class FocusRingPainter final {
	public:
		static void Draw(Renderer& renderer, tinycore::Rect rect, const FocusRingStyle& style);
	};
}