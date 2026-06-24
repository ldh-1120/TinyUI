#include <TinyUI/Builder/UIBuilder.h>

namespace tinyui {
	UIBuilder::UIBuilder() {
		m_root = std::make_unique<Widget>(MakeWidgetKey(L"root"));
		m_root->MarkVisited();
	}

	void UIBuilder::BeginFrame() {
		m_stack.clear();

		m_root->ClearVisitedRecursive();
		m_root->MarkVisited();

		UIBuilderStackEntry rootEntry { };
		rootEntry.widget = m_root.get();
		rootEntry.nextChildIndex = 0;

		m_stack.push_back(rootEntry);

		m_building = true;
	}

	void UIBuilder::EndFrame() {
		if (!m_building)
			return;

		while (m_stack.size() > 1)
			m_stack.pop_back();

		m_root->RemoveUnvisitedChildren();
		m_root->ArrangeTree();

		m_building = false;
	}

	Widget& UIBuilder::GetRoot() {
		return *m_root;
	}

	const Widget& UIBuilder::GetRoot() const {
		return *m_root;
	}

	Widget& UIBuilder::Current() {
		return *GetCurrentParent();
	}

	Widget& UIBuilder::BeginColumn(std::wstring_view keyText) {
		Widget& widget = Begin<Widget>(keyText);

		EnsureStackLayout(widget, LayoutDirection::Vertical);

		return widget;
	}

	Widget& UIBuilder::BeginRow(std::wstring_view keyText) {
		Widget& widget = Begin<Widget>(keyText);

		EnsureStackLayout(widget, LayoutDirection::Horizontal);

		return widget;
	}

	ButtonResult UIBuilder::Button(std::wstring_view keyText, std::wstring_view text) {
		ButtonOptions options { };

		return Button(keyText, text, options);
	}

	ButtonResult UIBuilder::Button(std::wstring_view keyText, std::wstring_view text, const ButtonOptions& options) {
		tinyui::Button& button = Begin<tinyui::Button>(keyText);

		button.SetText(text);
		button.SetOptions(options);

		if (button.GetLayoutStyle().preferredSize.width <= 0.0f)
			button.GetLayoutStyle().preferredSize.width = 96.0f;

		if (button.GetLayoutStyle().preferredSize.height <= 0.0f)
			button.GetLayoutStyle().preferredSize.height = 36.0f;

		ButtonResult result { };
		result.clicked = button.TakeClicked();
		result.hovered = button.IsHovered();
		result.down = button.IsDown();

		End();

		return result;
	}

	void UIBuilder::EndColumn() {
		End();
	}

	void UIBuilder::EndRow() {
		End();
	}

	void UIBuilder::End() {
		if (m_stack.size() <= 1)
			return;

		m_stack.pop_back();
	}

	Widget* UIBuilder::GetCurrentParent() {
		if (m_stack.empty())
			return m_root.get();

		return m_stack.back().widget;
	}

	std::size_t UIBuilder::GetCurrentChildIndex() const {
		if (m_stack.empty())
			return 0;

		return m_stack.back().nextChildIndex;
	}

	void UIBuilder::AdvanceCurrentChildIndex() {
		if (m_stack.empty())
			return;

		++m_stack.back().nextChildIndex;
	}

	void UIBuilder::EnsureStackLayout(Widget& widget, LayoutDirection direction) {
		StackLayout* layout = widget.GetLayoutAs<StackLayout>();
		if (!layout) {
			widget.SetLayout(std::make_unique<StackLayout>(direction));
			return;
		}

		layout->SetDirection(direction);
	}
}