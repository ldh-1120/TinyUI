#pragma once

#include <TinyCore/Input/InputState.h>

#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Rendering/FocusRingPainter.h>
#include <TinyUI/Widgets/Widget.h>

#include <vector>

namespace tinyui {
	enum class FocusReason {
		Mouse, Keyboard, Programmatic
	};

	class UIManager {
	public:
		UIManager() = default;
		~UIManager() = default;

		UIManager(const UIManager&) = delete;
		UIManager& operator=(const UIManager&) = delete;

		bool Update(Widget& root, float deltaTime);

		void Synchronize(Widget& root);

		void ProcessInput(Widget& root, const tinycore::InputState& input);
		void Paint(Widget& root, Renderer& renderer, const Theme& theme);

		Widget* GetHoveredWidget() const;
		Widget* GetFocusedWidget() const;
		Widget* GetCapturedWidget() const;

		Widget* HitTest(Widget& root, tinycore::Vec2 position) const;
		bool IsMouseInteractiveAt(Widget& root, tinycore::Vec2 position) const;

		void SetTooltipDelay(float delay);
		float GetTooltipDelay() const;

	private:
		void SetHoveredWidget(Widget* widget);
		void SetFocusedWidget(Widget* widget, FocusReason reason);

		Widget* FindFocusableAncestor(Widget* widget) const;
		void CollectFocusableWidgets(Widget& widget, std::vector<Widget*>& widgets) const;
		void MoveFocus(Widget& root, bool forward);
		void ProcessKeyboardInput(Widget& root, const tinycore::InputState& input);

		Widget* FindTooltipWidget(Widget* widget) const;
		bool UpdateTooltip(float deltaTime);
		void PaintTooltip(Widget& root, Renderer& renderer, const Theme& theme);
		void PaintFocusRing(Widget& root, Renderer& renderer, const Theme& theme);

	private:
		Widget* m_hoveredWidget = nullptr;
		Widget* m_focusedWidget = nullptr;
		Widget* m_capturedWidget = nullptr;
		Widget* m_tooltipTargetWidget = nullptr;
		Widget* m_tooltipVisibleWidget = nullptr;

		tinycore::Vec2 m_lastMousePosition { };
		tinycore::Vec2 m_tooltipAnchorPosition { };

		float m_tooltipTimer = 0.f;
		float m_tooltipDelay = 1.2f;

		float m_tooltipOpacity = 0.0f;
		float m_tooltipFadeInSpeed = 12.f;
		float m_tooltipFadeOutSpeed = 10.f;

		bool m_showFocusRing = false;
	};
}