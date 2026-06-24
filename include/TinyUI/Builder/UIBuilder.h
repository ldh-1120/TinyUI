#pragma once

#include <TinyUI/Core/WidgetKey.h>
#include <TinyUI/Layout/StackLayout.h>
#include <TinyUI/Widgets/Widget.h>
#include <TinyUI/Widgets/Button.h>

namespace tinyui {
	struct UIBuilderStackEntry {
		Widget* widget = nullptr;
		std::size_t nextChildIndex = 0;
	};

	struct ButtonResult {
		bool clicked = false;
		bool hovered = false;
		bool down = false;
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

		Widget& BeginColumn(std::wstring_view keyText);
		Widget& BeginRow(std::wstring_view keyText);

		ButtonResult Button(std::wstring_view keyText, std::wstring_view text);
		ButtonResult Button(std::wstring_view keyText, std::wstring_view text, const ButtonOptions& options);

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