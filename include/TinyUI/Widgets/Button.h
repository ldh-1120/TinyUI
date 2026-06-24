#pragma once

#include <TinyUI/Widgets/Widget.h>

#include <string>
#include <string_view>

namespace tinyui {
	struct ButtonOptions {
		float cornerRadius = -1.0f;
		float borderThickness = -1.0f;
		float fontSize = -1.0f;

		tinycore::Thickness padding { };
		bool useCustomPadding = false;
	};

	class Button final : public Widget {
	public:
		explicit Button(std::wstring text = L"");

		void SetText(std::wstring_view text);
		const std::wstring& GetText() const;

		void SetOptions(const ButtonOptions& options);
		const ButtonOptions& GetOptions() const;

		bool IsDown() const;

		bool TakeClicked();

	protected:
		void OnPaint(PaintContext& context) override;
		void OnMouseDown(MouseEvent& event) override;
		void OnMouseUp(MouseEvent& event) override;

	private:
		std::wstring m_text { };

		ButtonOptions m_options { };

		bool m_down = false;
		bool m_clicked = false;
	};
}