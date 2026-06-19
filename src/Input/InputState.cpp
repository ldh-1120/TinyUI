#include <TinyUI/Input/InputState.h>

namespace tinyui {
	size_t InputState::ToIndex(MouseButton button) {
		return static_cast<size_t>(button);
	}

	void InputState::ResetFrameState() {
		m_mousePressed.fill(false);
		m_mouseReleased.fill(false);
		m_mouseWheelDelta = 0.f;
	}

	void InputState::SetMousePosition(Vec2 position) {
		m_mousePositon = position;
	}

	void InputState::SetMouseButton(MouseButton button, bool down) {
		const size_t index = ToIndex(button);
		if (index >= MouseButtonCount)
			return;

		const bool wasDown = m_mouseDown[index];
		if (down && !wasDown)
			m_mousePressed[index] = true;

		if (!down && wasDown)
			m_mouseReleased[index] = true;

		m_mouseDown[index] = down;
	}

	void InputState::AddMouseWheelDelta(float delta) {
		m_mouseWheelDelta += delta;
	}

	Vec2 InputState::GetMousePosition() const {
		return m_mousePositon;
	}

	bool InputState::IsMouseOver(Rect rect) const {
		return rect.Contains(m_mousePositon);
	}

	bool InputState::IsMouseDown(MouseButton button) const {
		const size_t index = ToIndex(button);
		if (index >= MouseButtonCount)
			return false;

		return m_mouseDown[index];
	}

	bool InputState::WasMousePressed(MouseButton button) const {
		const size_t index = ToIndex(button);
		if (index >= MouseButtonCount)
			return false;

		return m_mousePressed[index];
	}

	bool InputState::WasMouseReleased(MouseButton button) const {
		const size_t index = ToIndex(button);
		if (index >= MouseButtonCount)
			return false;

		return m_mouseReleased[index];
	}

	bool InputState::IsAnyMouseButtonDown() const {
		for (bool down : m_mouseDown)
			if (down)
				return true;

		return false;
	}

	float InputState::GetMouseWheelDelta() const {
		return m_mouseWheelDelta;
	}
}