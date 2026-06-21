#include <TinyUI/Context/UIContext.h>

#include <cassert>
#include <utility>

namespace tinyui {
	void UIContext::BeginFrame(Renderer& renderer) {
		m_renderer = &renderer;
		m_hoveredId = WidgetId::Invalid();
	}

	void UIContext::BeginFrame(Renderer& renderer, double timeSeconds) {
		m_renderer = &renderer;
		m_hoveredId = WidgetId::Invalid();

		m_mouseBlockers.clear();
		m_overlayCallbacks.clear();
		if (!m_hasTime) {
			m_deltaTimeSeconds = 0.;
			m_hasTime = true;
		} else {
			m_deltaTimeSeconds = timeSeconds - m_timeSeconds;
			if (m_deltaTimeSeconds < 0.)
				m_deltaTimeSeconds = 0.;
		}

		m_timeSeconds = timeSeconds;
	}

	void UIContext::EndFrame() {
		m_input.ResetFrameState();
		m_renderer = nullptr;
	}

	double UIContext::GetTimeSeconds() const {
		return m_timeSeconds;
	}

	double UIContext::GetDeltaTimeSeconds() const {
		return m_deltaTimeSeconds;
	}

	Renderer& UIContext::GetRenderer() {
		assert(m_renderer != nullptr);
		return *m_renderer;
	}

	const Renderer& UIContext::GetRenderer() const {
		assert(m_renderer != nullptr);
		return *m_renderer;
	}

	InputState& UIContext::GetInput() {
		return m_input;
	}

	const InputState& UIContext::GetInput() const {
		return m_input;
	}

	ThemeRegistry& UIContext::GetThemeRegistry() {
		return m_themeRegistry;
	}

	const ThemeRegistry& UIContext::GetThemeRegistry() const {
		return m_themeRegistry;
	}

	Theme& UIContext::GetTheme() {
		return m_themeRegistry.GetCurrentTheme();
	}

	const Theme& UIContext::GetTheme() const {
		return m_themeRegistry.GetCurrentTheme();
	}

	bool UIContext::SetTheme(std::wstring_view id) {
		return m_themeRegistry.SetCurrentTheme(id);
	}

	void UIContext::SetHoveredId(WidgetId id) {
		m_hoveredId = id;
	}

	void UIContext::SetActiveId(WidgetId id) {
		m_activeId = id;
		m_activeMouseButton = MouseButton::Left;
	}

	void UIContext::SetActiveId(WidgetId id, MouseButton button) {
		m_activeId = id;
		m_activeMouseButton = button;
	}

	void UIContext::SetFocusedId(WidgetId id) {
		m_focusedId = id;
	}

	void UIContext::ClearHoveredId() {
		m_hoveredId = WidgetId::Invalid();
	}

	void UIContext::ClearActiveId() {
		m_activeId = WidgetId::Invalid();
		m_activeMouseButton = MouseButton::Left;
	}

	void UIContext::ClearFocusedId() {
		m_focusedId = WidgetId::Invalid();
		m_textInputId = WidgetId::Invalid();
	}

	WidgetId UIContext::GetHoveredId() const {
		return m_hoveredId;
	}

	WidgetId UIContext::GetActiveId() const {
		return m_activeId;
	}

	WidgetId UIContext::GetFocusedId() const {
		return m_focusedId;
	}

	bool UIContext::HasActiveId() const {
		return m_activeId.IsValid();
	}

	bool UIContext::CanActive(WidgetId id) const {
		return !m_activeId.IsValid() || m_activeId == id;
	}

	bool UIContext::IsHovered(WidgetId id) const {
		return m_hoveredId == id;
	}

	bool UIContext::IsActive(WidgetId id) const {
		return m_activeId == id;
	}

	bool UIContext::IsFocused(WidgetId id) const {
		return m_focusedId == id;
	}

	MouseButton UIContext::GetActiveMouseButton() const {
		return m_activeMouseButton;
	}

	bool UIContext::IsMouseOver(Rect rect) const {
		if (!m_input.IsMouseOver(rect))
			return false;

		return !IsMouseBlockedAt(m_input.GetMousePosition());
	}

	void UIContext::AddMouseBlocker(Rect rect) {
		m_mouseBlockers.push_back(rect);
	}

	bool UIContext::IsMouseBlockedAt(Vec2 point) const {
		for (std::size_t index = 0; index < m_mouseBlockers.size(); ++index) {
			if (m_mouseBlockers[index].Contains(point))
				return true;
		}

		return false;
	}

	void UIContext::AddOverlay(std::function<void(Renderer&)> drawCallback) {
		m_overlayCallbacks.push_back(std::move(drawCallback));
	}

	void UIContext::DrawOverlays() {
		if (!m_renderer)
			return;

		for (std::size_t index = 0; index < m_overlayCallbacks.size(); ++index)
			m_overlayCallbacks[index](*m_renderer);
	}

	bool UIContext::UpdateTooltip(WidgetId id, bool hovered, double delaySeconds) {
		if (!hovered) {
			if (m_tooltipTargetId == id) {
				m_tooltipTargetId = WidgetId::Invalid();
				m_tooltipStartTime = 0.;
			}

			return false;
		}

		if (m_tooltipTargetId != id) {
			m_tooltipTargetId = id;
			m_tooltipStartTime = m_timeSeconds;

			return false;
		}

		return m_timeSeconds - m_tooltipStartTime >= delaySeconds;
	}

	void UIContext::SetViewportSize(Size size) {
		m_viewportSize = size;
	}

	Size UIContext::GetViewportSize() const {
		return m_viewportSize;
	}

	void UIContext::SetTextInputId(WidgetId id) {
		m_textInputId = id;
	}

	void UIContext::ClearTextInputId() {
		m_textInputId = WidgetId::Invalid();
	}

	WidgetId UIContext::GetTextInputId() const {
		return m_textInputId;
	}

	bool UIContext::HasTextInputId() const {
		return m_textInputId.IsValid();
	}

	bool UIContext::IsTextInputActive(WidgetId id) const {
		return m_textInputId == id;
	}	
}