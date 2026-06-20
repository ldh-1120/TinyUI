#include <TinyUI/Widgets/Dropdown.h>

namespace tinyui {
	static int ClampInt(int value, int minValue, int maxValue) {
		if (value < minValue)
			return minValue;

		if (value > maxValue)
			return maxValue;

		return value;
	}

	static int GetVisibleItemCount(int itemCount, int maxVisibleItems) {
		int visibleItemCount = itemCount;
		if (visibleItemCount > maxVisibleItems)
			visibleItemCount = maxVisibleItems;

		if (visibleItemCount < 0)
			visibleItemCount = 0;

		return visibleItemCount;
	}

	static int GetInitialHighlightIndex(int selectedIndex, int itemCount) {
		if (itemCount <= 0)
			return -1;

		if (selectedIndex >= 0 && selectedIndex < itemCount)
			return selectedIndex;

		return 0;
	}

	static int MoveHighlightIndex(int currentIndex, int itemCount, int direction, bool wrap) {
		if (itemCount <= 0)
			return -1;

		if (currentIndex < 0 || currentIndex >= itemCount)
			currentIndex = 0;

		int nextIndex = currentIndex + direction;
		if (wrap) {
			if (nextIndex < 0)
				nextIndex = itemCount - 1;

			if (nextIndex >= itemCount)
				nextIndex = 0;

			return nextIndex;
		}

		return ClampInt(nextIndex, 0, itemCount - 1);
	}

	static void EnsureHighlightItemVisible(DropdownState& state, int itemCount, int visibleItemCount) {
		if (itemCount <= 0 || visibleItemCount <= 0) {
			state.firstVisibleIndex = 0;
			return;
		}

		if (state.highlightedIndex < 0)
			return;

		if (state.highlightedIndex < state.firstVisibleIndex)
			state.firstVisibleIndex = state.highlightedIndex;

		if (state.highlightedIndex >= state.firstVisibleIndex + visibleItemCount)
			state.firstVisibleIndex = state.highlightedIndex - visibleItemCount + 1;

		int maxFirstVisibleIndex = itemCount - visibleItemCount;
		if (maxFirstVisibleIndex < 0)
			maxFirstVisibleIndex = 0;

		state.firstVisibleIndex = ClampInt(state.firstVisibleIndex, 0, maxFirstVisibleIndex);
	}

	DropdownStyle DropdownStyle::FromTheme(const Theme& theme) {
		DropdownStyle style {};

		style.backgroundColor = theme.dropdown.background;
		style.hoveredBackgroundColor = theme.dropdown.backgroundHovered;
		style.openedBackgroundColor = theme.dropdown.backgroundOpened;

		style.borderColor = theme.dropdown.border;
		style.focusedBorderColor = theme.dropdown.borderFocused;

		style.textColor = theme.dropdown.text;
		style.placeholderColor = theme.dropdown.placeholder;
		style.arrowColor = theme.dropdown.arrow;

		style.menuBackgroundColor = theme.dropdown.menuBackground;
		style.itemHoveredColor = theme.dropdown.itemHovered;
		style.itemSelectedColor = theme.dropdown.itemSelected;

		return style;
	}


	DropdownResult Dropdown(UIContext& context, WidgetId id, Rect rect, DropdownState& state, const std::wstring_view* items, int itemCount, int& selectedIndex, DropdownOptions options) {
		return Dropdown(context, id, rect, state, items, itemCount, selectedIndex, options, DropdownStyle::FromTheme(context.GetTheme()));
	}

	DropdownResult Dropdown(UIContext& context, WidgetId id, Rect rect, DropdownState& state, const std::wstring_view* items, int itemCount, int& selectedIndex, DropdownOptions options, DropdownStyle style) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();

		DropdownResult result { };
		result.hovered = context.IsMouseOver(rect);

		if (itemCount <= 0)
			selectedIndex = -1;
		else if (selectedIndex >= itemCount)
			selectedIndex = itemCount - 1;

		if (result.hovered)
			context.SetHoveredId(id);

		for (MouseButton button : AllMouseButtons) {
			if (!HasMouseButton(options.acceptedButtons, button) || !context.CanActive(id))
				continue;

			if (result.hovered && input.WasMousePressed(button)) {
				context.SetActiveId(id, button);
				context.SetFocusedId(id);
				context.ClearTextInputId();
			}
		}

		if (context.IsActive(id)) {
			MouseButton activeButton = context.GetActiveMouseButton();
			if (input.WasMouseReleased(activeButton)) {
				if (result.hovered)
					state.open = !state.open;

				if (state.open) {
					state.highlightedIndex = GetInitialHighlightIndex(selectedIndex, itemCount);
					state.firstVisibleIndex = 0;
				}

				context.ClearActiveId();
			}
		}

		bool focused = context.IsFocused(id);
		if (focused && itemCount > 0) {
			int visibleItemCount = GetVisibleItemCount(itemCount, options.maxVisibleItems);
			if (input.WasKeyPressedOrRepeated(KeyCode::Down)) {
				if (!state.open) {
					state.open = true;
					state.highlightedIndex = GetInitialHighlightIndex(selectedIndex, itemCount);
				} else
					state.highlightedIndex = MoveHighlightIndex(state.highlightedIndex, itemCount, 1, options.wrapNavigation);

				EnsureHighlightItemVisible(state, itemCount, visibleItemCount);
			}

			if (input.WasKeyPressedOrRepeated(KeyCode::Up)) {
				if (!state.open) {
					state.open = true;
					state.highlightedIndex = GetInitialHighlightIndex(selectedIndex, itemCount);
				} else
					state.highlightedIndex = MoveHighlightIndex(state.highlightedIndex, itemCount, -1, options.wrapNavigation);

				EnsureHighlightItemVisible(state, itemCount, visibleItemCount);
			}

			if (input.WasKeyPressed(KeyCode::Enter) || input.WasKeyPressed(KeyCode::Space)) {
				if (!state.open) {
					state.open = true;
					state.highlightedIndex = GetInitialHighlightIndex(selectedIndex, itemCount);

					EnsureHighlightItemVisible(state, itemCount, visibleItemCount);
				} else {
					if (state.highlightedIndex >= 0 && state.highlightedIndex < itemCount) {
						selectedIndex = state.highlightedIndex;
						result.changed = true;
					}

					state.open = false;
				}
			}

			if (options.closeOnEscape && input.WasKeyPressed(KeyCode::Escape))
				state.open = false;
		}

		Color backgroundColor = style.backgroundColor;
		if (state.open)
			backgroundColor = style.openedBackgroundColor;
		else if (result.hovered)
			backgroundColor = style.hoveredBackgroundColor;

		Color borderColor = style.borderColor;
		if (context.IsFocused(id))
			borderColor = style.focusedBorderColor;

		renderer.FillRect(rect, backgroundColor, style.radius);
		renderer.DrawRect(rect, borderColor, style.borderThickness, style.radius);

		std::wstring_view displayText = options.placeholder;
		Color displayTextColor = style.placeholderColor;
		if (selectedIndex >= 0 && selectedIndex <= itemCount) {
			displayText = items[selectedIndex];
			displayTextColor = style.textColor;
		}

		Rect textRect { rect.x + style.paddingX, rect.y, rect.w - style.paddingX * 2.f - 18.f, rect.h };
		renderer.DrawTextBox(displayText, textRect, displayTextColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);

		float arrowCenterX = rect.x + rect.w - style.paddingX - 7.f;
		float arrowCenterY = rect.y + rect.h * 0.5f;
		if (state.open) {
			renderer.DrawLine({ arrowCenterX - 4.f, arrowCenterY + 2.f }, { arrowCenterX, arrowCenterY - 2.f }, style.arrowColor, 1.5f);
			renderer.DrawLine({ arrowCenterX + 4.f, arrowCenterY + 2.f }, { arrowCenterX, arrowCenterY - 2.f }, style.arrowColor, 1.5f);
		} else {
			renderer.DrawLine({ arrowCenterX - 4.f, arrowCenterY - 2.f }, { arrowCenterX, arrowCenterY + 2.f }, style.arrowColor, 1.5f);
			renderer.DrawLine({ arrowCenterX + 4.f, arrowCenterY - 2.f }, { arrowCenterX, arrowCenterY + 2.f }, style.arrowColor, 1.5f);
		}

		if (state.open && itemCount > 0) {
			int visibleItemCount = GetVisibleItemCount(itemCount, options.maxVisibleItems);
			EnsureHighlightItemVisible(state, itemCount, visibleItemCount);

			Rect menuRect { rect.x, rect.y + rect.h + style.menuGap, rect.w, style.itemHeight * static_cast<float>(visibleItemCount) };
			bool menuHovered = input.IsMouseOver(menuRect);

			Rect fullBlocker { -100000.f, -100000.f, 200000.f, 200000.f };
			context.AddMouseBlocker(fullBlocker);
			if (input.WasMousePressed(MouseButton::Left) && !result.hovered && !menuHovered)
				state.open = false;

			int hoveredItemIndex = -1;
			if (state.open) {
				for (int visibleIndex = 0; visibleIndex < visibleItemCount; ++visibleIndex) {
					int itemIndex = state.firstVisibleIndex + visibleIndex;
					Rect itemRect { menuRect.x, menuRect.y + style.itemHeight * static_cast<float>(visibleIndex), menuRect.w, style.itemHeight };
					if (input.IsMouseOver(itemRect)) {
						hoveredItemIndex = itemIndex;
						state.highlightedIndex = itemIndex;
						if (input.WasMousePressed(MouseButton::Left)) {
							selectedIndex = itemIndex;
							state.open = false;

							context.SetFocusedId(id);
							context.ClearTextInputId();

							result.changed = true;
						}
					}
				}
			}

			if (state.open) {
				std::vector<std::wstring> visibleItems { };
				for (int visibleIndex = 0; visibleIndex < visibleItemCount; ++visibleIndex) {
					int itemIndex = state.firstVisibleIndex + visibleIndex;
					visibleItems.push_back(std::wstring(items[itemIndex]));
				}

				int drawSelectedIndex = selectedIndex;
				int drawHighlightedIndex = state.highlightedIndex;
				int drawFirstVisibleIndex = state.firstVisibleIndex;
				context.AddOverlay([menuRect, style, visibleItems, drawSelectedIndex, drawHighlightedIndex, drawFirstVisibleIndex](Renderer& overlayRenderer) {
					overlayRenderer.FillRect(menuRect, style.menuBackgroundColor, style.radius);
					overlayRenderer.DrawRect(menuRect, style.borderColor, style.borderThickness, style.radius);

					overlayRenderer.PushClip(menuRect);

					const int visibleCount = static_cast<int>(visibleItems.size());
					for (int visibleIndex = 0; visibleIndex < visibleCount; ++visibleIndex) {
						int itemIndex = drawFirstVisibleIndex + visibleIndex;
						Rect itemRect { menuRect.x, menuRect.y + style.itemHeight * static_cast<float>(visibleIndex), menuRect.w, style.itemHeight };

						bool itemSelected = itemIndex == drawSelectedIndex;
						bool itemHighlighted = itemIndex == drawHighlightedIndex;
						if (itemSelected)
							overlayRenderer.FillRect(itemRect, style.itemSelectedColor, 0.f);
						else if (itemHighlighted)
							overlayRenderer.FillRect(itemRect, style.itemHoveredColor, 0.f);

						Rect itemTextRect { itemRect.x + style.paddingX, itemRect.y, itemRect.w - style.paddingX * 2.f, itemRect.h };
						std::wstring_view itemText(visibleItems[visibleIndex].data(), visibleItems[visibleIndex].size());
						overlayRenderer.DrawTextBox(itemText, itemTextRect, style.textColor, style.fontSize, TextAlign::Left);
					}

					overlayRenderer.PopClip();
				});
			}
		}

		result.open = state.open;
		result.selectedIndex = selectedIndex;

		return result;
	}
}