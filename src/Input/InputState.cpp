#include <TinyUI/Input/InputState.h>

namespace tinyui {
	size_t InputState::ToIndex(MouseButton button) {
		return static_cast<size_t>(button);
	}

	std::size_t InputState::ToIndex(KeyCode key) {
		return static_cast<size_t>(key);
	}

	void InputState::ResetFrameState() {
		m_mousePressed.fill(false);
		m_mouseReleased.fill(false);
		m_mouseWheelDelta = 0.f;

		m_keyPressed.fill(false);
		m_keyReleased.fill(false);
		m_textInput.clear();
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

	void InputState::SetKey(KeyCode key, bool down) {
		const std::size_t index = ToIndex(key);
		if (index == 0 || index >= KeyCodeCount)
			return;

		const bool wasDown = m_keyDown[index];
		if (down && !wasDown)
			m_keyPressed[index] = true;

		if (!down && wasDown)
			m_keyReleased[index] = true;

		m_keyDown[index] = down;
	}

	void InputState::AddTextCharacter(wchar_t character) {
		m_textInput.push_back(character);
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

	bool InputState::IsKeyDown(KeyCode key) const {
		const std::size_t index = ToIndex(key);
		if (index == 0 || index >= KeyCodeCount)
			return false;

		return m_keyDown[index];
	}

	bool InputState::WasKeyPressed(KeyCode key) const {
		const std::size_t index = ToIndex(key);
		if (index == 0 || index >= KeyCodeCount)
			return false;

		return m_keyPressed[index];
	}

	bool InputState::WasKeyReleased(KeyCode key) const {
		const std::size_t index = ToIndex(key);
		if (index == 0 || index >= KeyCodeCount)
			return false;

		return m_keyReleased[index];
	}

	bool InputState::IsControlDown() const {
		return IsKeyDown(KeyCode::Control);
	}

	bool InputState::IsShiftDown() const {
		return IsKeyDown(KeyCode::Shift);
	}

	bool InputState::IsAltDown() const {
		return IsKeyDown(KeyCode::Alt);
	}

	bool InputState::WasShortcutPressed(KeyCode key, bool control, bool shift, bool alt) const {
		if (!WasKeyPressed(key))
			return false;

		if (IsControlDown() != control)
			return false;

		if (IsShiftDown() != shift)
			return false;

		if (IsAltDown() != alt)
			return false;

		return true;
	}

	const std::wstring& InputState::GetTextInput() const {
		return m_textInput;
	}
}