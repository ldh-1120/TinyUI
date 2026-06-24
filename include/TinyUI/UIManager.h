#pragma once

#include <TinyCore/Input/InputState.h>

#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
	class UIManager {
	public:
		UIManager() = default;
		~UIManager() = default;

		UIManager(const UIManager&) = delete;
		UIManager& operator=(const UIManager&) = delete;

		void Synchronize(Widget& root);

		void ProcessInput(Widget& root, const tinycore::InputState& input);
		void Paint(Widget& root, Renderer& renderer, const Theme& theme);

		Widget* GetHoveredWidget() const;
		Widget* GetFocusedWidget() const;
		Widget* GetCapturedWidget() const;

	private:
		void SetHoveredWidget(Widget* widget);
		void SetFocusedWidget(Widget* widget);

	private:
		Widget* m_hoveredWidget = nullptr;
		Widget* m_focusedWidget = nullptr;
		Widget* m_capturedWidget = nullptr;
	};
}