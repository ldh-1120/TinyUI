#pragma once

#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/InputState.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/ThemeRegistry.h>

#include <string_view>

namespace tinyui {
	class UIContext {
	public:
		void BeginFrame(Renderer& renderer);
		void EndFrame();

		Renderer& GetRenderer();
		const Renderer& GetRenderer() const;

		InputState& GetInput();
		const InputState& GetInput() const;

		ThemeRegistry& GetThemeRegistry();
		const ThemeRegistry& GetThemeRegistry() const;

		Theme& GetTheme();
		const Theme& GetTheme() const;

		bool SetTheme(std::wstring_view id);

		void SetHoveredId(WidgetId id);
		void SetActiveId(WidgetId id);
		void SetActiveId(WidgetId id, MouseButton button);
		void SetFocusedId(WidgetId id);
		void SetTextInputId(WidgetId id);

		void ClearHoveredId();
		void ClearActiveId();
		void ClearFocusedId();
		void ClearTextInputId();

		WidgetId GetHoveredId() const;
		WidgetId GetActiveId() const;
		WidgetId GetFocusedId() const;
		WidgetId GetTextInputId() const;

		bool HasActiveId() const;
		bool HasTextInputId() const;
		bool CanActive(WidgetId id) const;

		bool IsHovered(WidgetId id) const;
		bool IsActive(WidgetId id) const;
		bool IsFocused(WidgetId id) const;
		bool IsTextInputActive(WidgetId id) const;

		MouseButton GetActiveMouseButton() const;

	private:
		Renderer* m_renderer = nullptr;

		InputState m_input { };
		ThemeRegistry m_themeRegistry { };

		WidgetId m_hoveredId = WidgetId::Invalid();
		WidgetId m_activeId = WidgetId::Invalid();
		WidgetId m_focusedId = WidgetId::Invalid();
		WidgetId m_textInputId = WidgetId::Invalid();

		MouseButton m_activeMouseButton = MouseButton::Left;
	};
}