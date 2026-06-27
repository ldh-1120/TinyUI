#pragma once

#include <TinyUI/Widgets/Widget.h>
#include <TinyUI/Icons/ButtonIcon.h>

#include <string>
#include <string_view>

namespace tinyui {
	struct ButtonOptions {
		ButtonVariant variant = ButtonVariant::Secondary;

		ButtonStyle customStyle { };
		bool useCustomStyle = false;

		static ButtonOptions Variant(ButtonVariant variant) {
			ButtonOptions options { };
			options.variant = variant;
			return options;
		}

		static ButtonOptions Custom(const ButtonStyle& style) {
			ButtonOptions options;
			options.customStyle = style;
			options.useCustomStyle = true;
			return options;
		}
	};

	class Button final : public Widget {
	public:
		explicit Button(std::wstring text = L"");

		void SetIcon(ButtonIcon icon);
		ButtonIcon GetIcon() const;

		void SetText(std::wstring_view text);
		const std::wstring& GetText() const;

		void SetOptions(const ButtonOptions& options);
		const ButtonOptions& GetOptions() const;

		bool IsDown() const;

		bool TakeClicked();

		bool IsMouseInteractive() const override;

	protected:
		void OnPaint(PaintContext& context) override;
		void OnMouseDown(MouseEvent& event) override;
		void OnMouseUp(MouseEvent& event) override;

		bool OnUpdate(float deltaTime) override;

	private:
		ButtonStyle ResolveStyle(const Theme& theme) const;
		ButtonStateStyle ResolveStateStyle(const ButtonStyle& style) const;
		ButtonStateStyle BlendStateStyle(const ButtonStyle& style) const;

	private:
		std::wstring m_text { };
		ButtonIcon m_icon = ButtonIcon::None;

		ButtonOptions m_options { };

		bool m_down = false;
		bool m_clicked = false;

		float m_hoverT = 0.0f;
		float m_pressedT = 0.0f;
	};
}