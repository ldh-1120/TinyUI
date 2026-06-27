#include "pch.h"

#include <TinyUI/Builder/UIScope.h>

#include <Tinyui/Builder/UIBuilder.h>

namespace tinyui {
	UIScope::UIScope(UIBuilder* builder, Widget* widget) : m_builder(builder), m_widget(widget), m_active(builder != nullptr) { }

	UIScope::UIScope(UIScope&& other) noexcept : m_builder(other.m_builder), m_widget(other.m_widget), m_active(other.m_active) {
		other.m_builder = nullptr;
		other.m_widget = nullptr;
		other.m_active = false;
	}

	UIScope& UIScope::operator=(UIScope&& other) noexcept {
		if (this == &other)
			return *this;

		End();

		m_builder = other.m_builder;
		m_widget = other.m_widget;
		m_active = other.m_active;

		other.m_builder = nullptr;
		other.m_widget = nullptr;
		other.m_active = false;

		return *this;
	}

	UIScope::~UIScope() {
		End();
	}

	void UIScope::End() {
		if (!m_active || !m_builder)
			return;

		m_builder->End();

		m_active = false;
		m_widget = nullptr;
		m_builder = nullptr;
	}

	bool UIScope::IsActive() const {
		return m_active;
	}

	Widget& UIScope::GetWidget() const {
		return *m_widget;
	}
}