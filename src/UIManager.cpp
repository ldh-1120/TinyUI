#include "pch.h"

#include <TinyCore/Math/Numeric.h>
#include <TinyUI/UIManager.h>

namespace tinyui {
	static tinycore::Color ApplyOpacity(tinycore::Color color, float opacity) {
		opacity = tinycore::Clamp01(opacity);

		color.a *= opacity;

		return color;
	}

	bool UIManager::Update(Widget& root, float deltaTime) {
		bool needsRedraw = root.UpdateTree(deltaTime);
		if (UpdateTooltip(deltaTime))
			needsRedraw = true;

		return needsRedraw;
	}

	void UIManager::Synchronize(Widget& root) {
		if (m_hoveredWidget && !root.ContainsWidget(m_hoveredWidget))
			m_hoveredWidget = nullptr;

		if (m_focusedWidget && !root.ContainsWidget(m_focusedWidget))
			m_focusedWidget = nullptr;

		if (m_capturedWidget && !root.ContainsWidget(m_capturedWidget))
			m_capturedWidget = nullptr;

		if (m_tooltipTargetWidget && !root.ContainsWidget(m_tooltipTargetWidget)) {
			m_tooltipTargetWidget = nullptr;
			m_tooltipTimer = 0.0f;
		}

		if (m_tooltipVisibleWidget && !root.ContainsWidget(m_tooltipVisibleWidget)) {
			m_tooltipVisibleWidget = nullptr;
			m_tooltipOpacity = 0.0f;
		}

		if (m_focusedWidget && !root.ContainsWidget(m_focusedWidget))
			SetFocusedWidget(nullptr, FocusReason::Programmatic);
	}

	void UIManager::ProcessInput(Widget& root, const tinycore::InputState& input) {
		Synchronize(root);

		m_lastMousePosition = input.GetMousePosition();
		tinycore::Vec2 mousePosition = input.GetMousePosition();

		Widget* hitWidget = root.HitTest(mousePosition);
		SetHoveredWidget(hitWidget);
		if (input.WasMousePressed(tinycore::MouseButton::Left)) {
			SetFocusedWidget(hitWidget, FocusReason::Mouse);
			if (hitWidget) {
				MouseEvent event { };
				event.position = mousePosition;
				event.button = tinycore::MouseButton::Left;

				hitWidget->OnMouseDown(event);
				if (event.accepted)
					m_capturedWidget = hitWidget;
			}
			Widget* focusWidget = FindFocusableAncestor(hitWidget);
			SetFocusedWidget(focusWidget, FocusReason::Mouse);
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

		if (m_capturedWidget) {
			MouseEvent moveEvent { };
			moveEvent.position = mousePosition;

			m_capturedWidget->OnMouseMove(moveEvent);
		}

		ProcessKeyboardInput(root, input);
	}

	void UIManager::Paint(Widget& root, Renderer& renderer, const Theme& theme) {
		PaintContext context { renderer, theme };
		root.PaintTree(context);

		PaintFocusRing(root, renderer, theme);
		PaintTooltip(root, renderer, theme);
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

	Widget* UIManager::HitTest(Widget& root, tinycore::Vec2 position) const {
		return root.HitTest(position);
	}

	bool UIManager::IsMouseInteractiveAt(Widget& root, tinycore::Vec2 position) const {
		Widget* widget = root.HitTest(position);
		while (widget) {
			if (widget->IsVisible() && widget->IsEnabled() && widget->IsMouseInteractive())
				return true;

			widget = widget->GetParent();
		}

		return false;
	}

	void UIManager::SetTooltipDelay(float delay) {
		if (delay < 0.0f)
			delay = 0.0f;

		m_tooltipDelay = delay;
	}

	float UIManager::GetTooltipDelay() const {
		return m_tooltipDelay;
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

	void UIManager::SetFocusedWidget(Widget* widget, FocusReason reason) {
		if (m_focusedWidget == widget) {
			if (reason == FocusReason::Keyboard)
				m_showFocusRing = true;
			else if (reason == FocusReason::Mouse)
				m_showFocusRing = false;

			return;
		}

		if (m_focusedWidget) {
			m_focusedWidget->SetFocusedInternal(false);
			m_focusedWidget->OnBlur();
		}

		m_focusedWidget = widget;
		if (m_focusedWidget) {
			m_focusedWidget->SetFocusedInternal(true);
			m_focusedWidget->OnFocus();
		}

		if (!m_focusedWidget) {
			m_showFocusRing = false;
			return;
		}

		if (reason == FocusReason::Keyboard)
			m_showFocusRing = true;
		else if (reason == FocusReason::Mouse)
			m_showFocusRing = false;
	}

	Widget* UIManager::FindFocusableAncestor(Widget* widget) const {
		Widget* current = widget;
		while (current) {
			if (current->IsVisible() && current->IsEnabled() && current->IsFocusable())
				return current;

			current = current->GetParent();
		}

		return nullptr;
	}

	void UIManager::CollectFocusableWidgets(Widget& root, std::vector<Widget*>& widgets) const {
		if (!root.IsVisible() || !root.IsEnabled())
			return;

		if (root.IsFocusable())
			widgets.push_back(&root);

		for (std::size_t index = 0; index < root.GetChildCount(); ++index) {
			Widget* child = root.GetChild(index);
			if (child)
				CollectFocusableWidgets(*child, widgets);
		}
	}

	void UIManager::MoveFocus(Widget& root, bool forward) {
		std::vector<Widget*> widgets { };

		CollectFocusableWidgets(root, widgets);
		if (widgets.empty()) {
			SetFocusedWidget(nullptr, FocusReason::Keyboard);
			return;
		}

		if (!m_focusedWidget) {
			if (forward)
				SetFocusedWidget(widgets.front(), FocusReason::Keyboard);
			else
				SetFocusedWidget(widgets.back(), FocusReason::Keyboard);

			return;
		}

		std::size_t currentIndex = widgets.size();
		for (std::size_t index = 0; index < widgets.size(); ++index) {
			if (widgets[index] == m_focusedWidget) {
				currentIndex = index;
				break;
			}
		}

		if (currentIndex == widgets.size()) {
			SetFocusedWidget(widgets.front(), FocusReason::Keyboard);
			return;
		}

		std::size_t nextIndex = 0;
		if (forward) {
			nextIndex = currentIndex + 1;
			if (nextIndex >= widgets.size())
				nextIndex = 0;
		} else {
			if (currentIndex == 0)
				nextIndex = widgets.size() - 1;
			else
				nextIndex = currentIndex - 1;
		}

		SetFocusedWidget(widgets[nextIndex], FocusReason::Keyboard);
	}

	void UIManager::ProcessKeyboardInput(Widget& root, const tinycore::InputState& input) {
		if (input.WasKeyPressed(tinycore::KeyCode::Tab)) {
			bool forward = !input.IsKeyDown(tinycore::KeyCode::Shift);

			MoveFocus(root, forward);
			return;
		}

		if (!m_focusedWidget)
			return;

		if (!root.ContainsWidget(m_focusedWidget)) {
			SetFocusedWidget(nullptr, FocusReason::Keyboard);
			return;
		}

		if (input.WasKeyPressed(tinycore::KeyCode::Enter)) {
			if (m_focusedWidget)
				m_showFocusRing = true;

			KeyEvent event { };
			event.key = tinycore::KeyCode::Enter;
			event.repeated = false;

			m_focusedWidget->OnKeyDown(event);
		}

		if (input.WasKeyPressed(tinycore::KeyCode::Space)) {
			KeyEvent event { };
			event.key = tinycore::KeyCode::Space;
			event.repeated = false;

			m_focusedWidget->OnKeyDown(event);
		}

		if (input.WasKeyReleased(tinycore::KeyCode::Space)) {
			KeyEvent event { };
			event.key = tinycore::KeyCode::Space;
			event.repeated = false;

			m_focusedWidget->OnKeyUp(event);
		}
	}

	Widget* UIManager::FindTooltipWidget(Widget* widget) const {
		Widget* current = widget;
		while (current) {
			if (current->HasTooltip())
				return current;

			current = current->GetParent();
		}

		return nullptr;
	}

	bool UIManager::UpdateTooltip(float deltaTime) {
		Widget* nextTooltipWidget = FindTooltipWidget(m_hoveredWidget);

		bool needsRedraw = false;
		if (nextTooltipWidget != m_tooltipTargetWidget) {
			m_tooltipTargetWidget = nextTooltipWidget;
			m_tooltipTimer = 0.f;
			needsRedraw = true;
		}

		bool wantsVisible = false;
		if (m_tooltipTargetWidget) {
			if (m_tooltipTargetWidget == m_tooltipVisibleWidget)
				wantsVisible = true;
			else {
				m_tooltipTimer += deltaTime;
				needsRedraw = true;
				if (m_tooltipTimer >= m_tooltipDelay) {
					m_tooltipVisibleWidget = m_tooltipTargetWidget;
					m_tooltipAnchorPosition = m_lastMousePosition;
					m_tooltipOpacity = 0.f;
					wantsVisible = true;
					needsRedraw = true;
				}
			}
		}

		float targetOpacity = wantsVisible ? 1.f : 0.f;
		float speed = wantsVisible ? m_tooltipFadeInSpeed : m_tooltipFadeOutSpeed;

		float previousOpacity = m_tooltipOpacity;
		m_tooltipOpacity = tinycore::MoveTowards(m_tooltipOpacity, targetOpacity, speed * deltaTime);
		if (previousOpacity != m_tooltipOpacity)
			needsRedraw = true;

		if (!wantsVisible && m_tooltipOpacity <= 0.f)
			m_tooltipVisibleWidget = nullptr;

		return needsRedraw;
	}

	void UIManager::PaintTooltip(Widget& root, Renderer& renderer, const Theme& theme) {
		if (!m_tooltipVisibleWidget || m_tooltipOpacity <= 0.f)
			return;

		const std::wstring& text = m_tooltipVisibleWidget->GetTooltip();
		if (text.empty())
			return;

		tinycore::Size textSize = renderer.MeasureTextWrapped(text, theme.tooltip.fontSize, theme.tooltip.maxWidth);
		tinycore::Thickness padding = theme.tooltip.padding;

		float tooltipWidth = textSize.width + padding.left + padding.right;
		float tooltipHeight = textSize.height + padding.top + padding.bottom;

		tinycore::Rect rootRect = root.GetRect();
		tinycore::Vec2 position { m_tooltipAnchorPosition.x, m_tooltipAnchorPosition.y + 18.f };
		if (position.x + tooltipWidth + 8.f > rootRect.x + rootRect.w)
			position.x = rootRect.x + rootRect.w - tooltipWidth - 8.f;

		if (position.y + tooltipHeight > rootRect.y + rootRect.h)
			position.y = m_tooltipAnchorPosition.y - tooltipHeight - 12.f;

		if (position.x < rootRect.x + 8.f)
			position.x = rootRect.x + 8.f;

		if (position.y < rootRect.y + 8.f)
			position.y = rootRect.y + 8.f;

		tinycore::Rect tooltipRect { position.x, position.y, tooltipWidth, tooltipHeight };
		renderer.FillRect(tooltipRect, ApplyOpacity(theme.tooltip.background, m_tooltipOpacity), theme.tooltip.cornerRadius);
		if (theme.tooltip.borderThickness > 0.f)
			renderer.DrawRect(tooltipRect, ApplyOpacity(theme.tooltip.border, m_tooltipOpacity), theme.tooltip.borderThickness, theme.tooltip.cornerRadius);

		tinycore::Rect textRect { tooltipRect.x + padding.left, tooltipRect.y + padding.top, tooltipRect.x - padding.left - padding.right, tooltipRect.y - padding.top - padding.bottom };
		renderer.DrawTextBox(text, textRect, ApplyOpacity(theme.tooltip.text, m_tooltipOpacity), theme.tooltip.fontSize, TextAlign::Left, TextWrap::Wrap);
	}

	void UIManager::PaintFocusRing(Widget& root, Renderer& renderer, const Theme& theme) {
		if (!m_focusedWidget || !root.ContainsWidget(m_focusedWidget) || !m_focusedWidget->ShouldDrawFocusRing() || !m_showFocusRing)
			return;

		FocusRingStyle style = m_focusedWidget->GetFocusRingStyle(theme);
		FocusRingPainter::Draw(renderer, m_focusedWidget->GetRect(), style);
	}
}