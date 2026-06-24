#pragma once

#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>

namespace tinyui {
	struct PaintContext {
		Renderer& renderer;
		const Theme& theme;
	};
}