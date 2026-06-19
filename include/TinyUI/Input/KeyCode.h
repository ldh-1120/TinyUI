#pragma once

#include <cstddef>

namespace tinyui {
	enum class KeyCode {
		Unknown = 0,
		Backspace, Tab, Enter, Escape, Space, 
		Delete, Home, End, PageUp, PageDown,
		Left, Right, Up, Down,
		Control, Shift, Alt,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

		Count
	};

	inline constexpr std::size_t KeyCodeCount = static_cast<std::size_t>(KeyCode::Count);
}