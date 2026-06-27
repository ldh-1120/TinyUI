#pragma once

#include <TinyUI/Core/WidgetKey.h>
#include <TinyUI/Builder/UIScope.h>
#include <TinyUI/Layout/StackLayout.h>
#include <TinyUI/Widgets/Widget.h>
#include <TinyUI/Widgets/Button.h>
#include <TinyUI/Widgets/Label.h>
#include <TinyUI/Widgets/Panel.h>
#include <TinyUI/Widgets/Spacer.h>
#include <TinyUI/Widgets/Separator.h>
#include <TinyUI/Widgets/Slider.h>

namespace tinyui {
	struct UIBuilderStackEntry {
		Widget* widget = nullptr;
		std::size_t nextChildIndex = 0;
	};

	struct ButtonResult {
		bool clicked = false;
		bool hovered = false;
		bool down = false;

		Button* widget = nullptr;
		Button& GetWidget() const {
			return *widget;
		}
	};

	struct SliderResult {
		float value = 0.f;
		bool changed = false;

		Slider* widget = nullptr;
		Slider& GetWidget() const {
			return *widget;
		}
	};

	class UIBuilder {
	public:
		UIBuilder();
		~UIBuilder() = default;

		UIBuilder(const UIBuilder&) = delete;
		UIBuilder& operator=(const UIBuilder&) = delete;

		void BeginFrame();
		void EndFrame();

		Widget& GetRoot();
		const Widget& GetRoot() const;

		Widget& Current();

		template<typename WidgetType, typename... Args>
		WidgetType& Begin(std::wstring_view keyText, Args&&... args) {
			WidgetKey key = MakeWidgetKey(keyText);

			Widget* parent = GetCurrentParent();
			std::size_t childIndex = GetCurrentChildIndex();

			AdvanceCurrentChildIndex();

			WidgetType* widget = parent->GetOrCreateChildAt<WidgetType>(childIndex, key, std::forward<Args>(args)...);

			widget->MarkVisited();

			UIBuilderStackEntry entry { };
			entry.widget = widget;
			entry.nextChildIndex = 0;

			m_stack.push_back(entry);

			return *widget;
		}

		[[nodiscard]] UIScope Row(std::wstring_view keyText);
		[[nodiscard]] UIScope Column(std::wstring_view keyText);

		[[nodiscard]] UIScope PanelRow(std::wstring_view keyText, std::wstring_view title);
		[[nodiscard]] UIScope PanelColumn(std::wstring_view keyText, std::wstring_view title);

		[[nodiscard]] UIScope PanelRow(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options);
		[[nodiscard]] UIScope PanelColumn(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options);

		Widget& BeginColumn(std::wstring_view keyText);
		Widget& BeginRow(std::wstring_view keyText);

		Panel& BeginPanel(std::wstring_view keyText, std::wstring_view title);
		Panel& BeginPanel(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options);

		Panel& BeginPanelRow(std::wstring_view keyText, std::wstring_view title);
		Panel& BeginPanelRow(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options);

		Panel& BeginPanelColumn(std::wstring_view keyText, std::wstring_view title);
		Panel& BeginPanelColumn(std::wstring_view keyText, std::wstring_view title, const PanelOptions& options);

		ButtonResult Button(std::wstring_view keyText, std::wstring_view text);
		ButtonResult Button(std::wstring_view keyText, std::wstring_view text, const ButtonOptions& options);

		ButtonResult IconButton(std::wstring_view keyText, ButtonIcon icon);
		ButtonResult IconButton(std::wstring_view keyText, ButtonIcon icon, const ButtonOptions& options);

		tinyui::Label& Label(std::wstring_view keyText, std::wstring_view text);
		tinyui::Label& Label(std::wstring_view keyText, std::wstring_view text, const LabelOptions& options);

		SliderResult Slider(std::wstring_view keyText, float value, float minimum, float maximum);
		SliderResult Slider(std::wstring_view keyText, float value, float minimum, float maximum, const SliderOptions& options);

		void Spacer(std::wstring_view keyText);
		void Spacer(std::wstring_view keyText, float stretch);
		void Spacer(std::wstring_view keyText, tinycore::Size preferredSize, float stretch);

		void Gap(std::wstring_view keyText, float size);

		tinyui::Separator& Separator(std::wstring_view keyText);
		tinyui::Separator& Separator(std::wstring_view keyText, const SeparatorOptions& options);

		void EndPanel();
		void EndColumn();
		void EndRow();

		void End();

	private:
		Widget* GetCurrentParent();
		std::size_t GetCurrentChildIndex() const;
		void AdvanceCurrentChildIndex();

		void EnsureStackLayout(Widget& widget, LayoutDirection direction);

	private:
		std::unique_ptr<Widget> m_root { };

		std::vector<UIBuilderStackEntry> m_stack { };

		bool m_building = false;
	};
}