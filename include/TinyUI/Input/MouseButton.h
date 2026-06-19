#pragma once

#include <cstddef>
#include <initializer_list>

namespace tinyui {
	enum class MouseButton {
		Left = 0,
		Right, Middle, X1, X2,

		Count
	};

	using MouseButtonMask = unsigned int;

	inline constexpr std::size_t MouseButtonCount = static_cast<std::size_t>(MouseButton::Count);

	inline constexpr MouseButtonMask MouseButtonBit(MouseButton button) {
		return 1u << static_cast<unsigned int>(button);
	}

	inline constexpr bool HasMouseButton(MouseButtonMask mask, MouseButton button) {
		return (mask & MouseButtonBit(button)) != 0;
	}

	inline MouseButtonMask MakeMouseButtonMask(std::initializer_list<MouseButton> buttons) {
		MouseButtonMask mask = 0;
		for (MouseButton button : buttons)
			mask |= MouseButtonBit(button);

		return mask;
	}

	inline constexpr MouseButton AllMouseButtons[] {
		MouseButton::Left, MouseButton::Right, MouseButton::Middle, MouseButton::X1, MouseButton::X2
	};
}