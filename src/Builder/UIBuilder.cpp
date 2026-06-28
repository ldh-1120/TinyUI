#include "pch.h"

#include <TinyUI/Builder/UIBuilder.h>

namespace tinyui {
	UIBuilder::UIBuilder() {
		m_root = std::make_unique<Widget>(MakeWidgetKey(L"root"));

		m_root->SetLayout(std::make_unique<StackLayout>(
			LayoutDirection::Vertical
		));
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

		tinycore::Size rootSize = m_root->GetRect().GetSize();
		m_root->MeasureTree(rootSize);
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

#pragma region Scope

	UIScope UIBuilder::Row(std::wstring_view keyText) {
		Widget& widget = BeginRow(keyText);

		return UIScope(this, &widget);
	}

	UIScope UIBuilder::Column(std::wstring_view keyText) {
		Widget& widget = BeginColumn(keyText);

		return UIScope(this, &widget);
	}

	UIScope UIBuilder::PanelRow(std::wstring_view keyText, std::wstring_view title) {
		Panel& panel = BeginPanelRow(keyText, title);

		return UIScope(this, &panel);
	}

	UIScope UIBuilder::PanelColumn(std::wstring_view keyText, std::wstring_view title) {
		Panel& panel = BeginPanelColumn(keyText, title);

		return UIScope(this, &panel);
	}

	UIScope UIBuilder::PanelRow(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options) {
		Panel& panel = BeginPanelRow(keyText, title, options);

		return UIScope(this, &panel);
	}

	UIScope UIBuilder::PanelColumn(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options) {
		Panel& panel = BeginPanelColumn(keyText, title, options);

		return UIScope(this, &panel);
	}

#pragma endregion

#pragma region Panel

	Panel& UIBuilder::BeginPanel(std::wstring_view keyText, std::wstring_view title) {
		PanelOptions options { };

		return BeginPanel(keyText, title, options);
	}

	Panel& UIBuilder::BeginPanel(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options) {
		Panel& panel = Begin<Panel>(keyText);

		panel.SetTitle(title);
		panel.SetOptions(options);

		EnsureStackLayout(panel, LayoutDirection::Vertical);

		StackLayout* layout = panel.GetLayoutAs<StackLayout>();
		if (layout) {
			tinycore::Thickness padding = options.useCustomContentPadding ? options.contentPadding : (options.showTitle ? tinycore::Thickness { 12.f, 0.f, 12.f, 12.f } : tinycore::Thickness::Uniform(16.0f));
			layout->SetPadding(padding);
			layout->SetGap(12.0f);
		}

		return panel;
	}

	Panel& UIBuilder::BeginPanelRow(std::wstring_view keyText, std::wstring_view title) {
		PanelOptions options { };

		return BeginPanelRow(keyText, title, options);
	}

	Panel& UIBuilder::BeginPanelRow(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options) {
		Panel& panel = BeginPanel(keyText, title, options);

		StackLayout* layout = panel.GetLayoutAs<StackLayout>();
		if (layout)
			layout->SetDirection(LayoutDirection::Horizontal);

		return panel;
	}

	Panel& UIBuilder::BeginPanelColumn(std::wstring_view keyText, std::wstring_view title) {
		PanelOptions options { };

		return BeginPanelColumn(keyText, title, options);
	}

	Panel& UIBuilder::BeginPanelColumn(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options) {
		Panel& panel = BeginPanel(keyText, title, options);

		StackLayout* layout = panel.GetLayoutAs<StackLayout>();
		if (layout)
			layout->SetDirection(LayoutDirection::Vertical);

		return panel;
	}

#pragma endregion

	ButtonResult UIBuilder::Button(std::wstring_view keyText, std::wstring_view text) {
		ButtonOptions options { };

		return Button(keyText, text, options);
	}

	ButtonResult UIBuilder::Button(std::wstring_view keyText, std::wstring_view text, const ButtonOptions& options) {
		tinyui::Button& button = Begin<tinyui::Button>(keyText);

		button.SetText(text);
		button.SetIcon(ButtonIcon::None);
		button.SetOptions(options);

		ButtonResult result { };
		result.clicked = button.TakeClicked();
		result.hovered = button.IsHovered();
		result.down = button.IsDown();
		result.widget = &button;

		End();

		return result;
	}

	ButtonResult UIBuilder::IconButton(std::wstring_view keyText, ButtonIcon icon) {
		ButtonOptions options { };

		return IconButton(keyText, icon, options);
	}

	ButtonResult UIBuilder::IconButton(std::wstring_view keyText, ButtonIcon icon, const ButtonOptions& options) {
		tinyui::Button& button = Begin<tinyui::Button>(keyText);

		button.SetText(L"");
		button.SetIcon(icon);
		button.SetOptions(options);

		ButtonResult result { };
		result.clicked = button.TakeClicked();
		result.hovered = button.IsHovered();
		result.down = button.IsDown();
		result.widget = &button;

		End();

		return result;
	}

	tinyui::Label& UIBuilder::Label(std::wstring_view keyText, std::wstring_view text) {
		LabelOptions options { };

		return Label(keyText, text, options);
	}

	tinyui::Label& UIBuilder::Label(std::wstring_view keyText, std::wstring_view text, const LabelOptions& options) {
		tinyui::Label& label = Begin<tinyui::Label>(keyText);

		label.SetText(text);
		label.SetOptions(options);

		if (label.GetLayoutStyle().preferredSize.height <= 0.0f) {
			if (options.variant == LabelVariant::Heading)
				label.GetLayoutStyle().preferredSize.height = 30.0f;
			else
				label.GetLayoutStyle().preferredSize.height = 24.0f;
		}

		End();

		return label;
	}

	SliderResult UIBuilder::Slider(std::wstring_view keyText, float value, float minimum, float maximum) {
		SliderOptions options { };
		return Slider(keyText, value, minimum, maximum, options);
	}

	SliderResult UIBuilder::Slider(std::wstring_view keyText, float value, float minimum, float maximum, const SliderOptions& options) {
		tinyui::Slider& slider = Begin<tinyui::Slider>(keyText);
		slider.SetRange(minimum, maximum);
		slider.SetOptions(options);
		if (!slider.IsDragging() && !slider.HasPendingChange())
			slider.SetValue(value);

		SliderResult result { };
		result.value = slider.GetValue();
		result.changed = slider.TakeChanged();
		result.widget = &slider;

		End();

		return result;
	}

	CheckboxResult UIBuilder::Checkbox(std::wstring_view keyText, bool checked) {
		CheckboxOptions options { };

		return Checkbox(keyText, checked, options);
	}

	CheckboxResult UIBuilder::Checkbox(std::wstring_view keyText, bool checked, const CheckboxOptions& options) {
		tinyui::Checkbox& checkbox = Begin<tinyui::Checkbox>(keyText);

		checkbox.SetOptions(options);
		if (!checkbox.HasPendingChange())
			checkbox.SetChecked(checked);

		CheckboxResult result { };
		result.checked = checkbox.IsChecked();
		result.changed = checkbox.TakeChanged();
		result.widget = &checkbox;

		End();

		return result;
	}

	TextBoxResult UIBuilder::TextBox(std::wstring_view keyText, std::wstring_view text) {
		TextBoxOptions options { };

		return TextBox(keyText, text, options);
	}

	TextBoxResult UIBuilder::TextBox(std::wstring_view keyText, std::wstring_view text, const TextBoxOptions& options) {
		tinyui::TextBox& textBox = Begin<tinyui::TextBox>(keyText);
		textBox.SetOptions(options);
		if (!textBox.HasPendingChange())
			textBox.SetText(text);

		TextBoxResult result { };
		result.text = textBox.GetText();
		result.changed = textBox.TakeChanged();
		result.widget = &textBox;

		End();

		return result;
	}

	void UIBuilder::Spacer(std::wstring_view keyText) {
		Spacer(keyText, 1.0f);
	}

	void UIBuilder::Spacer(std::wstring_view keyText, float stretch) {
		tinyui::Spacer& spacer = Begin<tinyui::Spacer>(keyText);

		spacer.SetPreferredSize({ 0.0f, 0.0f });
		spacer.SetStretch(stretch);

		End();
	}

	void UIBuilder::Spacer(std::wstring_view keyText, tinycore::Size preferredSize, float stretch) {
		tinyui::Spacer& spacer = Begin<tinyui::Spacer>(keyText);

		spacer.SetPreferredSize(preferredSize);
		spacer.SetStretch(stretch);

		End();
	}

	void UIBuilder::Gap(std::wstring_view keyText, float size) {
		Widget& parent = Current();

		StackLayout* layout = parent.GetLayoutAs<StackLayout>();

		if (layout && layout->GetDirection() == LayoutDirection::Horizontal) {
			Spacer(keyText, { size, 0.0f }, 0.0f);
			return;
		}

		Spacer(keyText, { 0.0f, size }, 0.0f);
	}

	tinyui::Separator& UIBuilder::Separator(std::wstring_view keyText) {
		SeparatorOptions options { };

		return Separator(keyText, options);
	}

	tinyui::Separator& UIBuilder::Separator(std::wstring_view keyText, const SeparatorOptions& options) {
		Widget& parent = Current();

		LayoutDirection parentDirection = LayoutDirection::Vertical;

		StackLayout* parentLayout = parent.GetLayoutAs<StackLayout>();
		if (parentLayout)
			parentDirection = parentLayout->GetDirection();

		tinyui::Separator& separator = Begin<tinyui::Separator>(keyText);

		separator.SetOptions(options);

		float thickness = options.thickness > 0.0f ? options.thickness : 1.0f;
		if (parentDirection == LayoutDirection::Horizontal) {
			separator.SetDirection(SeparatorDirection::Vertical);

			separator.GetLayoutStyle().preferredSize = { thickness, 0.0f };
			separator.GetLayoutStyle().horizontalAlignment = LayoutAlignment::Center;
			separator.GetLayoutStyle().verticalAlignment = LayoutAlignment::Stretch;
		} else {
			separator.SetDirection(SeparatorDirection::Horizontal);

			separator.GetLayoutStyle().preferredSize = { 0.0f, thickness };
			separator.GetLayoutStyle().horizontalAlignment = LayoutAlignment::Stretch;
			separator.GetLayoutStyle().verticalAlignment = LayoutAlignment::Center;
		}

		End();

		return separator;
	}

	void UIBuilder::EndPanel() {
		End();
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