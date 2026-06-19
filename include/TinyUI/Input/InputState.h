#pragma once

#include <TinyUI/Core/Types.h>
#include <TinyUI/Input/MouseButton.h>

#include <array>
#include <cstddef>

namespace tinyui {
	class InputState {
	public:
		void ResetFrameState();

		void SetMousePosition(Vec2 position);
		void SetMouseButton(MouseButton button, bool down);
		void AddMouseWheelDelta(float delta);

		Vec2 GetMousePosition() const;

		bool IsMouseOver(Rect rect) const;

		bool IsMouseDown(MouseButton button) const;
		bool WasMousePressed(MouseButton button) const;
		bool WasMouseReleased(MouseButton button) const;

		bool IsAnyMouseButtonDown() const;

		float GetMouseWheelDelta() const;

	private:
		static std::size_t ToIndex(MouseButton button);

	private:
		Vec2 m_mousePositon { };

		std::array<bool, MouseButtonCount> m_mouseDown { };
		std::array<bool, MouseButtonCount> m_mousePressed { };
		std::array<bool, MouseButtonCount> m_mouseReleased { };

		float m_mouseWheelDelta = 0.f;
	};
}