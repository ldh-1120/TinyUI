#include <TinyUI/Widgets/Label.h>

namespace tinyui {
	LabelStyle LabelStyle::FromTheme(const Theme& theme, LabelVariant variant) {
		LabelStyle style { };
		if (variant == LabelVariant::Normal) {
			style.textColor = theme.label.text;
			style.fontSize = 14.0f;
		}

		if (variant == LabelVariant::Muted) {
			style.textColor = theme.label.mutedText;
			style.fontSize = 13.0f;
		}

		if (variant == LabelVariant::Heading) {
			style.textColor = theme.label.headingText;
			style.fontSize = 16.0f;
		}

		return style;
	}

	LabelResult Label(UIContext& context, Rect rect, std::wstring_view text, LabelOptions options) {
		return Label(context, rect, text, options, LabelStyle::FromTheme(context.GetTheme(), options.variant));
	}

	LabelResult Label(UIContext& context, Rect rect, std::wstring_view text, LabelOptions options, LabelStyle style) {
		Renderer& renderer = context.GetRenderer();

		LabelResult result { };
		result.rect = rect;

		renderer.DrawTextBox(text, rect, style.textColor, style.fontSize, options.align, options.wrap);

		return result;
	}
}