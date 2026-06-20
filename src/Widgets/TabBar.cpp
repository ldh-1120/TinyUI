#include <TinyUI/Widgets/TabBar.h>

namespace tinyui {
	static float ClampFloat(float value, float minValue, float maxValue) {
		if (value < minValue)
			return minValue;

		if (value > maxValue)
			return maxValue;

		return value;
	}

	static int MoveTabIndex(int selectedIndex, int tabCount, int direction, bool wrap) {
		if (tabCount <= 0)
			return -1;

		if (selectedIndex < 0 || selectedIndex >= tabCount)
			selectedIndex = 0;

		int nextIndex = selectedIndex + direction;
		if (wrap) {
			if (nextIndex < 0)
				nextIndex = tabCount - 1;

			if (nextIndex >= tabCount)
				nextIndex = 0;

			return nextIndex;
		}

		if (nextIndex < 0)
			nextIndex = 0;

		if (nextIndex >= tabCount)
			nextIndex = tabCount - 1;

		return nextIndex;
	}

	TabBarStyle TabBarStyle::FromTheme(const Theme& theme) {
		TabBarStyle style {};

		style.backgroundColor = theme.tabBar.background;

		style.tabNormalColor = theme.tabBar.tabNormal;
		style.tabHoveredColor = theme.tabBar.tabHovered;
		style.tabPressedColor = theme.tabBar.tabPressed;
		style.tabSelectedColor = theme.tabBar.tabSelected;

		style.borderColor = theme.tabBar.border;
		style.accentColor = theme.tabBar.accent;

		style.textColor = theme.tabBar.text;
		style.selectedTextColor = theme.tabBar.selectedText;

		return style;
	}

	TabBarResult TabBar(UIContext& context, WidgetId id, Rect rect, TabBarState& state, const std::wstring_view* tabs, int tabCount, int& selectedIndex, TabBarOptions options) {
		return TabBar(context, id, rect, state, tabs, tabCount, selectedIndex, options, TabBarStyle::FromTheme(context.GetTheme()));
	}

	TabBarResult TabBar(UIContext& context, WidgetId id, Rect rect, TabBarState& state, const std::wstring_view* tabs, int tabCount, int& selectedIndex, TabBarOptions options, TabBarStyle style) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();

		TabBarResult result { };
		result.hovered = context.IsMouseOver(rect);

		if (tabCount <= 0) {
			selectedIndex = -1;
			state.pressedIndex = -1;
			return result;
		}

		if (selectedIndex < 0)
			selectedIndex = 0;

		if (selectedIndex >= tabCount)
			selectedIndex = tabCount - 1;

		if (result.hovered)
			context.SetHoveredId(id);

		if (options.keyboardNavigation && context.IsFocused(id)) {
			if (input.WasKeyPressedOrRepeated(KeyCode::Left)) {
				int nextIndex = MoveTabIndex(selectedIndex, tabCount, -1, options.wrapNavigation);
				if (nextIndex != selectedIndex) {
					selectedIndex = nextIndex;
					result.changed = true;
				}
			}

			if (input.WasKeyPressedOrRepeated(KeyCode::Right)) {
				int nextIndex = MoveTabIndex(selectedIndex, tabCount, 1, options.wrapNavigation);
				if (nextIndex != selectedIndex) {
					selectedIndex = nextIndex;
					result.changed = true;
				}
			}
		}

		renderer.FillRect(rect, style.backgroundColor, style.radius);
		
		float currentX = rect.x;
		for (int index = 0; index < tabCount; ++index) {
			float tabWidth = 0.f;
			if (options.stretchTabs)
				tabWidth = (rect.w - (tabCount - 1) * style.gap) / static_cast<float>(tabCount);
			else {
				Size textSize = renderer.MeasureText(tabs[index], style.fontSize);
				tabWidth = textSize.width + style.paddingX * 2.f;
				tabWidth = ClampFloat(tabWidth, style.minTabWidth, style.maxTabWidth);
			}

			Rect tabRect { currentX, rect.y, tabWidth, rect.h };
			bool tabHovered = context.IsMouseOver(tabRect);
			bool tabSelected = index == selectedIndex;
			bool tabPressed = context.IsActive(id) && state.pressedIndex == index && input.IsMouseDown(context.GetActiveMouseButton());
			if (tabHovered)
				result.hoveredIndex = index;

			for (MouseButton button : AllMouseButtons) {
				if (!HasMouseButton(options.acceptedButtons, button) || !context.CanActive(id))
					continue;

				if (tabHovered && input.WasMousePressed(button)) {
					context.SetActiveId(id, button);
					context.SetFocusedId(id);
					context.ClearTextInputId();

					state.pressedIndex = index;
				}
			}

			if (context.IsActive(id) && state.pressedIndex == index) {
				MouseButton activeButton = context.GetActiveMouseButton();
				if (input.WasMouseReleased(activeButton)) {
					if (tabHovered && selectedIndex != index) {
						selectedIndex = index;
						result.changed = true;
					}

					state.pressedIndex = -1;
					context.ClearActiveId();
				}
			}

			Color tabColor = style.tabNormalColor;
			if (tabSelected)
				tabColor = style.tabSelectedColor;
			else if (tabPressed)
				tabColor = style.tabPressedColor;
			else if (tabHovered)
				tabColor = style.tabHoveredColor;

			renderer.FillRect(tabRect, tabColor, style.radius);
			renderer.DrawRect(tabRect, style.borderColor, style.borderThickness, style.radius);
			if (tabSelected) {
				Rect indicatorRect { tabRect.x + 8.f, tabRect.y + tabRect.h - style.activeIndicatorHeight, tabRect.w - 16.f, style.activeIndicatorHeight };
				renderer.FillRect(indicatorRect, style.accentColor, style.activeIndicatorHeight * 0.5f);
			}

			Color textColor = style.textColor;
			if (tabSelected)
				textColor = style.selectedTextColor;

			renderer.DrawTextBox(tabs[index], tabRect, textColor, style.fontSize, TextAlign::Center);

			currentX += tabWidth + style.gap;
		}

		if (!input.IsMouseDown(MouseButton::Left) && state.pressedIndex != -1)
			state.pressedIndex = -1;

		result.selectedIndex = selectedIndex;

		return result;
	}
}