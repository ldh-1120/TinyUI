#include "pch.h"

#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
	Widget::Widget(WidgetKey key) : m_key(key) { }

	WidgetKey Widget::GetKey() const {
		return m_key;
	}

	void Widget::SetKey(WidgetKey key) {
		m_key = key;
	}

	Widget* Widget::GetParent() const {
		return m_parent;
	}

	Rect Widget::GetRect() const {
		return m_rect;
	}

	void Widget::SetRect(Rect rect) {
		m_rect = rect;
	}

	Rect Widget::GetContentRect() const {
		return m_rect;
	}

	bool Widget::IsVisible() const {
		return m_visible;
	}

	void Widget::SetVisible(bool visible) {
		m_visible = visible;
	}

	bool Widget::IsEnabled() const {
		return m_enabled;
	}

	void Widget::SetEnabled(bool enabled) {
		m_enabled = enabled;
	}

	bool Widget::IsHovered() const {
		return m_hovered;
	}

	bool Widget::IsFocused() const {
		return m_focused;
	}

	bool Widget::WasVisited() const {
		return m_visited;
	}

	void Widget::MarkVisited() {
		m_visited = true;
	}

	bool Widget::UpdateTree(float deltaTime) {
		if (!IsVisible())
			return false;

		bool needsRedraw = OnUpdate(deltaTime);
		for (std::size_t index = 0; index < m_children.size(); ++index) {
			Widget* child = m_children[index].get();
			if (child && child->UpdateTree(deltaTime))
				needsRedraw = true;
		}

		return needsRedraw;
	}

	void Widget::ArrangeTree() {
		if (m_layout)
			m_layout->Arrange(*this);

		for (std::size_t index = 0; index < m_children.size(); ++index)
			m_children[index]->ArrangeTree();
	}

	Widget* Widget::HitTest(tinycore::Vec2 position) {
		if (!m_visible || !m_enabled)
			return nullptr;

		for (std::size_t index = m_children.size(); index > 0; --index) {
			Widget* child = m_children[index - 1].get();
			Widget* hitWidget = child->HitTest(position);
			if (hitWidget)
				return hitWidget;
		}

		if (!m_rect.Contains(position))
			return nullptr;

		return this;
	}

	void Widget::PaintTree(PaintContext& context) {
		if (!m_visible)
			return;

		OnPaint(context);
		for (std::size_t index = 0; index < m_children.size(); ++index)
			m_children[index]->PaintTree(context);
	}

	Widget* Widget::FindChild(WidgetKey key) {
		for (std::size_t index = 0; index < m_children.size(); ++index) {
			if (m_children[index]->GetKey() == key)
				return m_children[index].get();
		}

		return nullptr;
	}

	const Widget* Widget::FindChild(WidgetKey key) const {
		for (std::size_t index = 0; index < m_children.size(); ++index) {
			if (m_children[index]->GetKey() == key)
				return m_children[index].get();
		}

		return nullptr;
	}

	void Widget::RemoveChild(WidgetKey key) {
		std::vector<std::unique_ptr<Widget>>::iterator childIterator =
			std::find_if(m_children.begin(), m_children.end(),
				[key](const std::unique_ptr<Widget>& child) -> bool {
			return child->GetKey() == key;
		});

		if (childIterator == m_children.end())
			return;

		(*childIterator)->OnRemoved();
		m_children.erase(childIterator);
	}

	void Widget::ClearVisitedRecursive() {
		m_visited = false;

		for (std::unique_ptr<Widget>& child : m_children)
			child->ClearVisitedRecursive();
	}

	void Widget::RemoveUnvisitedChildren() {
		for (std::unique_ptr<Widget>& child : m_children)
			child->RemoveUnvisitedChildren();

		std::vector<std::unique_ptr<Widget>>::iterator removeBegin =
			std::remove_if(m_children.begin(), m_children.end(),
				[](const std::unique_ptr<Widget>& child) -> bool {
			if (child->WasVisited())
				return false;

			child->OnRemoved();
			return true;
		});

		m_children.erase(removeBegin, m_children.end());
	}

	bool Widget::ContainsWidget(const Widget* widget) const {
		if (this == widget)
			return true;

		for (std::size_t index = 0; index < m_children.size(); ++index) {
			if (m_children[index]->ContainsWidget(widget))
				return true;
		}

		return false;
	}

	LayoutStyle& Widget::GetLayoutStyle() {
		return m_layoutStyle;
	}

	const LayoutStyle& Widget::GetLayoutStyle() const {
		return m_layoutStyle;
	}

	void Widget::SetLayout(std::unique_ptr<Layout> layout) {
		m_layout = std::move(layout);
	}

	Layout* Widget::GetLayout() {
		return m_layout.get();
	}

	const Layout* Widget::GetLayout() const {
		return m_layout.get();
	}

	std::size_t Widget::GetChildCount() const {
		return m_children.size();
	}

	Widget* Widget::GetChild(std::size_t index) {
		if (index >= m_children.size())
			return nullptr;

		return m_children[index].get();
	}

	const Widget* Widget::GetChild(std::size_t index) const {
		if (index >= m_children.size())
			return nullptr;

		return m_children[index].get();
	}

	void Widget::SetHoveredInternal(bool hovered) {
		if (m_hovered == hovered)
			return;

		m_hovered = hovered;
		if (m_hovered)
			OnMouseEnter();
		else
			OnMouseLeave();
	}

	void Widget::SetFocusedInternal(bool focused) {
		if (m_focused = focused)
			return;

		m_focused = focused;
		if (m_focused)
			OnFocus();
		else
			OnBlur();
	}

	std::size_t Widget::FindChildIndex(WidgetKey key) const {
		for (std::size_t index = 0; index < m_children.size(); ++index) {
			if (m_children[index]->GetKey() == key)
				return index;
		}

		return InvalidChildIndex;
	}

	void Widget::MoveChildToIndex(std::size_t fromIndex, std::size_t toIndex) {
		if (fromIndex >= m_children.size())
			return;

		if (toIndex >= m_children.size())
			toIndex = m_children.size() - 1;

		if (fromIndex == toIndex)
			return;

		std::unique_ptr<Widget> child = std::move(m_children[fromIndex]);
		m_children.erase(m_children.begin() + static_cast<std::ptrdiff_t>(fromIndex));
		if (toIndex > m_children.size())
			toIndex = m_children.size();

		m_children.insert(m_children.begin() + static_cast<std::ptrdiff_t>(toIndex), std::move(child));
	}

	bool Widget::IsMouseInteractive() const {
		return false;
	}

	void Widget::SetTooltip(std::wstring_view tooltip) {
		m_tooltip = tooltip;
	}

	void Widget::ClearTooltip() {
		m_tooltip.clear();
	}

	const std::wstring& Widget::GetTooltip() const {
		return m_tooltip;
	}

	bool Widget::HasTooltip() const {
		return !m_tooltip.empty();
	}

	void Widget::OnRemoved() { }

	bool Widget::OnUpdate(float deltaTime) {
		return false;
	}

	void Widget::OnPaint(PaintContext& context) { }

	void Widget::OnMouseEnter() { }

	void Widget::OnMouseLeave() { }

	void Widget::OnFocus() { }

	void Widget::OnBlur() { }

	void Widget::OnMouseDown(MouseEvent& event) { }

	void Widget::OnMouseUp(MouseEvent& event) { }
}