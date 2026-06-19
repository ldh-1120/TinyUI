#pragma once

#include <TinyUI/Core/Types.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Input/KeyCode.h>

#include <array>
#include <cstddef>
#include <string>

namespace tinyui {
	class InputState {
	public:
		void ResetFrameState();

		void SetMousePosition(Vec2 position);
		void SetMouseButton(MouseButton button, bool down);
		void AddMouseWheelDelta(float delta);

		void SetKey(KeyCode key, bool down);
		void AddTextCharacter(wchar_t character);

		Vec2 GetMousePosition() const;

		bool IsMouseOver(Rect rect) const;

		bool IsMouseDown(MouseButton button) const;
		bool WasMousePressed(MouseButton button) const;
		bool WasMouseReleased(MouseButton button) const;

		bool IsAnyMouseButtonDown() const;

		float GetMouseWheelDelta() const;

		bool IsKeyDown(KeyCode key) const;
		bool WasKeyPressed(KeyCode key) const;
		bool WasKeyReleased(KeyCode key) const;

		bool IsControlDown() const;
		bool IsShiftDown() const;
		bool IsAltDown() const;

		bool WasShortcutPressed(KeyCode key, bool control, bool shift, bool alt) const;

		const std::wstring& GetTextInput() const;

	private:
		static std::size_t ToIndex(MouseButton button);
		static std::size_t ToIndex(KeyCode key);

	private:
		Vec2 m_mousePositon { };

		std::array<bool, MouseButtonCount> m_mouseDown { };
		std::array<bool, MouseButtonCount> m_mousePressed { };
		std::array<bool, MouseButtonCount> m_mouseReleased { };

		float m_mouseWheelDelta = 0.f;

		std::array<bool, KeyCodeCount> m_keyDown { };
		std::array<bool, KeyCodeCount> m_keyPressed { };
		std::array<bool, KeyCodeCount> m_keyReleased { };

		std::wstring m_textInput { };
	};
}