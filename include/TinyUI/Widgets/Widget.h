#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyUI/Core/WidgetKey.h>
#include <TinyUI/Layout/Layout.h>
#include <TinyUI/Layout/LayoutStyle.h>

#include <memory>
#include <utility>
#include <vector>

using namespace tinycore;

namespace tinyui {
	class Widget {
	public:
		explicit Widget(WidgetKey key = WidgetKey::Invalid());
		virtual ~Widget() = default;

		Widget(const Widget&) = delete;
		Widget& operator=(const Widget&) = delete;

		WidgetKey GetKey() const;
		void SetKey(WidgetKey key);

		Widget* GetParent() const;

		Rect GetRect() const;
		void SetRect(Rect rect);

		bool IsVisible() const;
		void SetVisible(bool visible);

		bool IsEnabled() const;
		void SetEnabled(bool enabled);

		bool WasVisited() const;
		void MarkVisited();

		void ClearVisitedRecursive();
		void RemoveUnvisitedChildren();

		LayoutStyle& GetLayoutStyle();
		const LayoutStyle& GetLayoutStyle() const;

		void SetLayout(std::unique_ptr<Layout> layout);
		Layout* GetLayout();
		const Layout* GetLayout() const;

		template<typename LayoutType>
		LayoutType* GetLayoutAs() {
			return dynamic_cast<LayoutType*>(m_layout.get());
		}

		template<typename LayoutType>
		const LayoutType* GetLayoutAs() const {
			return dynamic_cast<const LayoutType*>(m_layout.get());
		}

		void ArrangeTree();

		Widget* FindChild(WidgetKey key);
		const Widget* FindChild(WidgetKey key) const;

		void RemoveChild(WidgetKey key);

		std::size_t GetChildCount() const;
		Widget* GetChild(std::size_t index);
		const Widget* GetChild(std::size_t index) const;

		template<typename WidgetType, typename... Args>
		WidgetType* GetOrCreateChild(WidgetKey key, Args&&... args) {
			Widget* existingChild = FindChild(key);
			if (existingChild) {
				WidgetType* typedChild = dynamic_cast<WidgetType*>(existingChild);
				if (typedChild)
					return typedChild;

				RemoveChild(key);
			}

			std::unique_ptr<WidgetType> child = std::make_unique<WidgetType>(std::forward<Args>(args)...);
			WidgetType* childPointer = child.get();
			childPointer->SetKey(key);
			childPointer->m_parent = this;

			m_children.push_back(std::move(child));

			return childPointer;
		}

	protected:
		virtual void OnRemoved();

	private:
		WidgetKey m_key = WidgetKey::Invalid();

		Widget* m_parent = nullptr;

		Rect m_rect { };

		bool m_visible = true;
		bool m_enabled = true;

		bool m_visited = false;

		LayoutStyle m_layoutStyle { };
		std::unique_ptr<Layout> m_layout { };

		std::vector<std::unique_ptr<Widget>> m_children { };
	};
}