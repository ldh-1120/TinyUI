#pragma once

#include <TinyCore/Core/Types.h>

namespace tinyui {
	class Widget;

	class Layout {
	public:
		virtual ~Layout() = default;

		virtual tinycore::Size Measure(Widget& parent, tinycore::Size availableSize) = 0;
		virtual void Arrange(Widget& parent) = 0;
	};
}