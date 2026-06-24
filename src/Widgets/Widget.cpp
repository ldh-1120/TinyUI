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

	bool Widget::WasVisited() const {
		return m_visited;
	}

	void Widget::MarkVisited() {
		m_visited = true;
	}

	void Widget::ArrangeTree() {
		if (m_layout)
			m_layout->Arrange(*this);

		for (std::size_t index = 0; index < m_children.size(); ++index)
			m_children[index]->ArrangeTree();
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

	void Widget::OnRemoved() { }
}