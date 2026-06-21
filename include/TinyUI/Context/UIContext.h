#pragma once

#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/InputState.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/ThemeRegistry.h>

#include <string_view>
#include <functional>
#include <vector>

namespace tinyui {
	class UIContext {
	public:
		void BeginFrame(Renderer& renderer);
		void BeginFrame(Renderer& renderer, double timeSeconds);
		void EndFrame();

		double GetTimeSeconds() const;
		double GetDeltaTimeSeconds() const;

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

		bool IsMouseOver(Rect rect) const;

		void AddMouseBlocker(Rect rect);
		bool IsMouseBlockedAt(Vec2 point) const;

		void AddOverlay(std::function<void(Renderer&)> drawCallback);
		void DrawOverlays();

		bool UpdateTooltip(WidgetId id, bool hovered, double delaySeconds);

		void SetViewportSize(Size size);
		Size GetViewportSize() const;

	private:
		Renderer* m_renderer = nullptr;

		InputState m_input { };
		ThemeRegistry m_themeRegistry { };

		WidgetId m_hoveredId = WidgetId::Invalid();
		WidgetId m_activeId = WidgetId::Invalid();
		WidgetId m_focusedId = WidgetId::Invalid();
		WidgetId m_textInputId = WidgetId::Invalid();
		WidgetId m_tooltipTargetId = WidgetId::Invalid();

		MouseButton m_activeMouseButton = MouseButton::Left;

		double m_timeSeconds = 0.;
		double m_deltaTimeSeconds = 0.;
		bool m_hasTime = false;

		double m_tooltipStartTime = 0.;

		Size m_viewportSize { };

		std::vector<Rect> m_mouseBlockers { };
		std::vector<std::function<void(Renderer&)>> m_overlayCallbacks { };
	};
}