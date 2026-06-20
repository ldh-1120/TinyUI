#include <TinyUI/Widgets/ScrollPanel.h>

namespace tinyui {
	static float ClampFloat(float value, float minValue, float maxValue) {
		if (value < minValue)
			return minValue;

		if (value > maxValue)
			return maxValue;

		return value;
	}

	ScrollPanelStyle ScrollPanelStyle::FromTheme(const Theme& theme) {
		ScrollPanelStyle style {};

		style.backgroundColor = theme.scrollPanel.background;
		style.borderColor = theme.scrollPanel.border;

		style.scrollbarTrackColor = theme.scrollPanel.scrollbarTrack;
		style.scrollbarThumbColor = theme.scrollPanel.scrollbarThumb;
		style.scrollbarThumbHoveredColor = theme.scrollPanel.scrollbarThumbHovered;

		return style;
	}

	ScrollPanelResult BeginScrollPanel(UIContext& context, WidgetId id, Rect rect, ScrollPanelState& state, ScrollPanelOptions options) {
		return BeginScrollPanel(context, id, rect, state, options, ScrollPanelStyle::FromTheme(context.GetTheme()));
	}

	ScrollPanelResult BeginScrollPanel(UIContext& context, WidgetId id, Rect rect, ScrollPanelState& state, ScrollPanelOptions options, ScrollPanelStyle style) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();

		ScrollPanelResult result { };
		result.rect = rect;
		result.hovered = input.IsMouseOver(rect);
		if (result.hovered)
			context.SetHoveredId(id);

		if (options.showBackground) {
			renderer.FillRect(rect, style.backgroundColor, style.radius);
			renderer.DrawRect(rect, style.borderColor, style.borderThickness, style.radius);
		}

		float scrollbarReservedWidth = 0.f;
		if (options.showScrollbar)
			scrollbarReservedWidth = style.scrollbarWidth + style.scrollbarGap;

		Rect visibleRect = { rect.x + style.padding, rect.y + style.padding, rect.w - style.padding * 2.f - scrollbarReservedWidth, rect.h - style.padding * 2.f };
		if (visibleRect.w < 0.f)
			visibleRect.w = 0.f;

		if (visibleRect.h < 0.f)
			visibleRect.h < 0.f;

		float contentHeight = options.contentHeight;
		if (contentHeight < visibleRect.h)
			contentHeight = visibleRect.h;

		float maxScrollY = contentHeight - visibleRect.h;
		if (maxScrollY < 0.f)
			maxScrollY = 0.f;

		if (result.hovered) {
			float wheelDelta = input.GetMouseWheelDelta();
			if (wheelDelta != 0.f) {
				state.scrollY -= wheelDelta * options.wheelSpeed;
				result.changed = true;
			}
		}

		state.scrollY = ClampFloat(state.scrollY, 0.f, maxScrollY);
		Rect scrollbarTrack { };
		Rect scrollbarThumb { };

		bool hasScrollbar = options.showScrollbar && maxScrollY > 0.f;
		float thumbHeight = 0.f;
		float availableThumbMove = 0.f;
		if (hasScrollbar) {
			scrollbarTrack = { rect.x + rect.w - style.padding - style.scrollbarWidth, visibleRect.y, style.scrollbarWidth, visibleRect.h };

			thumbHeight = visibleRect.h / contentHeight * scrollbarTrack.h;
			if (thumbHeight < 24.f)
				thumbHeight = 24.f;

			if (thumbHeight > scrollbarTrack.h)
				thumbHeight = scrollbarTrack.h;

			availableThumbMove = scrollbarTrack.h - thumbHeight;
			float scrollRatio = 0.f;
			if (maxScrollY > 0.f)
				scrollRatio = state.scrollY / maxScrollY;

			Rect scrollbarThumb { scrollbarTrack.x, scrollbarTrack.y + scrollRatio * availableThumbMove, scrollbarTrack.w, thumbHeight };
			result.scrollbarHovered = input.IsMouseOver(scrollbarThumb);
			if (result.scrollbarHovered && context.CanActive(id) && input.WasMousePressed(MouseButton::Left)) {
				context.SetActiveId(id, MouseButton::Left);
				context.SetFocusedId(id);
				context.ClearTextInputId();

				state.draggingThumb = true;
				state.dragStartMouseY = input.GetMousePosition().y;
				state.dragStartScrollY = state.scrollY;
			}
		}

		if (context.IsActive(id) && state.draggingThumb) {
			if (input.IsMouseDown(MouseButton::Left)) {
				float mouseDeltaY = input.GetMousePosition().y - state.dragStartMouseY;
				if (availableThumbMove > 0.f) {
					float scrollDelta = (mouseDeltaY / availableThumbMove) * maxScrollY;
					state.scrollY = state.dragStartScrollY + scrollDelta;
					state.scrollY = ClampFloat(state.scrollY, 0.f, maxScrollY);

					result.changed = true;
				}
			} else {
				state.draggingThumb = false;
				context.ClearActiveId();
			}
		} else if (!context.IsActive(id))
			state.draggingThumb = false;

		if (hasScrollbar) {
			float scrollRatio = 0.f;
			if (maxScrollY > 0.f)
				scrollRatio = state.scrollY / maxScrollY;

			scrollbarThumb = { scrollbarTrack.x, scrollbarTrack.y + scrollRatio * availableThumbMove, scrollbarTrack.w, thumbHeight };
			result.scrollbarHovered = input.IsMouseOver(scrollbarThumb);
			result.scrollbarDragging = state.draggingThumb;
			renderer.FillRect(scrollbarTrack, style.scrollbarTrackColor, style.scrollbarRadius);

			Color thumbColor = style.scrollbarThumbColor;
			if (result.scrollbarHovered || result.scrollbarDragging)
				thumbColor = style.scrollbarThumbHoveredColor;

			renderer.FillRect(scrollbarThumb, thumbColor, style.scrollbarRadius);
		}

		result.visibleRect = visibleRect;
		result.contentRect = { visibleRect.x, visibleRect.y - state.scrollY, visibleRect.w, contentHeight };
		result.scrollY = state.scrollY;
		result.maxScrollY = maxScrollY;

		renderer.PushClip(visibleRect);

		return result;
	}

	void EndScrollPanel(UIContext& context) {
		context.GetRenderer().PopClip();
	}
}