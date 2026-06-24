#pragma once

#include <TinyUI/Core/WidgetKey.h>
#include <TinyUI/Layout/StackLayout.h>
#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
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
			WidgetType* widget = parent->GetOrCreateChild<WidgetType>(key, std::forward<Args>(args)...);

			widget->MarkVisited();

			m_stack.push_back(widget);

			return *widget;
		}

		Widget& BeginColumn(std::wstring_view keyText);
		Widget& BeginRow(std::wstring_view keyText);

		void EndColumn();
		void EndRow();

		void End();

	private:
		Widget* GetCurrentParent();
		void EnsureStackLayout(Widget& widget, LayoutDirection direction);

	private:
		std::unique_ptr<Widget> m_root { };

		std::vector<Widget*> m_stack { };

		bool m_building = false;
	};
}