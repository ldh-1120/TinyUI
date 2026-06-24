#include <TinyUI/UIManager.h>

namespace tinyui {
	void UIManager::Synchronize(Widget& root) {
		if (m_hoveredWidget && !root.ContainsWidget(m_hoveredWidget))
			m_hoveredWidget = nullptr;

		if (m_focusedWidget && !root.ContainsWidget(m_focusedWidget))
			m_focusedWidget = nullptr;

		if (m_capturedWidget && !root.ContainsWidget(m_capturedWidget))
			m_capturedWidget = nullptr;
	}

	void UIManager::ProcessInput(Widget& root, const tinycore::InputState& input) {
		Synchronize(root);

		tinycore::Vec2 mousePosition = input.GetMousePosition();

		Widget* hitWidget = root.HitTest(mousePosition);
		SetHoveredWidget(hitWidget);
		if (input.WasMousePressed(tinycore::MouseButton::Left)) {
			m_capturedWidget = hitWidget;
			SetFocusedWidget(hitWidget);
			if (hitWidget) {
				MouseEvent event { };
				event.position = mousePosition;
				event.button = tinycore::MouseButton::Left;
				hitWidget->OnMouseDown(event);
			}
		}

		if (input.WasMouseReleased(tinycore::MouseButton::Left)) {
			Widget* targetWidget = m_capturedWidget;
			if (!targetWidget)
				targetWidget = hitWidget;

			if (targetWidget) {
				MouseEvent event { };
				event.position = mousePosition;
				event.button = tinycore::MouseButton::Left;

				targetWidget->OnMouseUp(event);
			}

			m_capturedWidget = nullptr;
		}
	}

	void UIManager::Paint(Widget& root, Renderer& renderer, const Theme& theme) {
		PaintContext context { renderer, theme };
		root.PaintTree(context);
	}

	Widget* UIManager::GetHoveredWidget() const {
		return m_hoveredWidget;
	}

	Widget* UIManager::GetFocusedWidget() const {
		return m_focusedWidget;
	}

	Widget* UIManager::GetCapturedWidget() const {
		return m_capturedWidget;
	}

	void UIManager::SetHoveredWidget(Widget* widget) {
		if (m_hoveredWidget == widget)
			return;

		if (m_hoveredWidget)
			m_hoveredWidget->SetHoveredInternal(false);

		m_hoveredWidget = widget;
		if (m_hoveredWidget)
			m_hoveredWidget->SetHoveredInternal(true);
	}

	void UIManager::SetFocusedWidget(Widget* widget) {
		if (m_focusedWidget == widget)
			return;

		if (m_focusedWidget)
			m_focusedWidget->SetFocusedInternal(false);

		m_focusedWidget = widget;
		if (m_focusedWidget)
			m_focusedWidget->SetFocusedInternal(true);
	}
}