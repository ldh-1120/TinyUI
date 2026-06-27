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
	}

	void UIManager::ProcessInput(Widget& root, const tinycore::InputState& input) {
		Synchronize(root);

		m_lastMousePosition = input.GetMousePosition();
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

	void UIManager::SetFocusedWidget(Widget* widget) {
		if (m_focusedWidget == widget)
			return;

		if (m_focusedWidget)
			m_focusedWidget->SetFocusedInternal(false);

		m_focusedWidget = widget;
		if (m_focusedWidget)
			m_focusedWidget->SetFocusedInternal(true);
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
}