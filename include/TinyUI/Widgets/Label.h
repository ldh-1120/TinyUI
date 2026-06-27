#pragma once

#include <TinyUI/Widgets/Widget.h>

#include <string>
#include <string_view>

namespace tinyui {
	enum class LabelVariant { Normal, Muted, Heading };

	struct LabelOptions {
		LabelVariant variant = LabelVariant::Normal;
		TextAlign align = TextAlign::Left;
		TextWrap wrap = TextWrap::NoWrap;

		float fontSize = -1.f;
	};

	class Label final : public Widget {
	public:
		explicit Label(std::wstring text = L"");

		void SetText(std::wstring_view text);
		const std::wstring& GetText() const;

		void SetOptions(const LabelOptions& options);
		const LabelOptions& GetOptions() const;

	protected:
		void OnPaint(PaintContext& context) override;

	private:
		tinycore::Color GetTextColor(const Theme& theme) const;
		float GetFontSize(const Theme& theme) const;

	private:
		std::wstring m_text { };
		LabelOptions m_options { };
	};
}