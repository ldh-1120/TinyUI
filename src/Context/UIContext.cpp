#include <TinyUI/Context/UIContext.h>

#include <cassert>

namespace tinyui {
	void UIContext::BeginFrame(Renderer& renderer) {
		m_renderer = &renderer;
		m_hoveredId = WidgetId::Invalid();
	}

	void UIContext::EndFrame() {
		m_input.ResetFrameState();
		m_renderer = nullptr;
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