#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyUI/Core/WidgetKey.h>
#include <TinyUI/Events/MouseEvent.h>
#include <TinyUI/Events/KeyEvent.h>
#include <TinyUI/Layout/Layout.h>
#include <TinyUI/Layout/LayoutStyle.h>
#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Widgets/Style/FocusRingStyle.h>

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

		virtual Rect GetContentRect() const;

		bool IsVisible() const;
		void SetVisible(bool visible);

		bool IsEnabled() const;
		void SetEnabled(bool enabled);

		bool IsHovered() const;
		bool IsFocused() const;

		bool WasVisited() const;
		void MarkVisited();

		bool UpdateTree(float deltaTime);

		void ClearVisitedRecursive();
		void RemoveUnvisitedChildren();

		bool ContainsWidget(const Widget* widget) const;

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

		Widget* HitTest(tinycore::Vec2 position);

		void PaintTree(PaintContext& context);

		Widget* FindChild(WidgetKey key);
		const Widget* FindChild(WidgetKey key) const;

		void RemoveChild(WidgetKey key);

		std::size_t GetChildCount() const;
		Widget* GetChild(std::size_t index);
		const Widget* GetChild(std::size_t index) const;

		template<typename WidgetType, typename... Args>
		WidgetType* GetOrCreateChild(WidgetKey key, Args&&... args) {
			return GetOrCreateChildAt<WidgetType>(m_children.size(), key, std::forward<Args>(args)...);
		}

		template<typename WidgetType, typename... Args>
		WidgetType* GetOrCreateChildAt(std::size_t desiredIndex, WidgetKey key, Args&&... args) {
			std::size_t existingIndex = FindChildIndex(key);
			if (existingIndex != InvalidChildIndex) {
				Widget* existingChild = m_children[existingIndex].get();
				WidgetType* typedChild = dynamic_cast<WidgetType*>(existingChild);
				if (typedChild) {
					MoveChildToIndex(existingIndex, desiredIndex);
					return typedChild;
				}

				RemoveChild(key);
			}

			std::unique_ptr<WidgetType> child = std::make_unique<WidgetType>(std::forward<Args>(args)...);
			WidgetType* childPointer = child.get();
			childPointer->SetKey(key);
			childPointer->m_parent = this;
			if (desiredIndex > m_children.size())
				desiredIndex = m_children.size();

			m_children.insert(m_children.begin() + static_cast<std::ptrdiff_t>(desiredIndex), std::move(child));

			return childPointer;
		}

		static constexpr std::size_t InvalidChildIndex = static_cast<std::size_t>(-1);

		std::size_t FindChildIndex(WidgetKey key) const;
		void MoveChildToIndex(std::size_t fromIndex, std::size_t toIndex);

		virtual bool IsMouseInteractive() const;

		void SetTooltip(std::wstring_view tooltip);
		void ClearTooltip();

		const std::wstring& GetTooltip() const;
		bool HasTooltip() const;

		virtual bool IsFocusable() const;

		virtual FocusRingStyle GetFocusRingStyle(const Theme& theme) const;
		virtual bool ShouldDrawFocusRing() const;

	protected:
		virtual void OnRemoved();

		virtual bool OnUpdate(float deltaTime);

		virtual void OnPaint(PaintContext& context);

		virtual void OnMouseEnter();
		virtual void OnMouseLeave();

		virtual void OnFocus();
		virtual void OnBlur();

		virtual void OnMouseDown(MouseEvent& event);
		virtual void OnMouseUp(MouseEvent& event);
		virtual void OnMouseMove(MouseEvent& event);

		virtual void OnKeyDown(KeyEvent& event);
		virtual void OnKeyUp(KeyEvent& event);

	private:
		friend class UIManager;

		void SetHoveredInternal(bool hovered);
		void SetFocusedInternal(bool focused);

	private:
		WidgetKey m_key = WidgetKey::Invalid();

		Widget* m_parent = nullptr;

		Rect m_rect { };

		bool m_visible = true;
		bool m_enabled = true;

		bool m_hovered = false;
		bool m_focused = false;

		bool m_visited = false;

		std::wstring m_tooltip { };

		LayoutStyle m_layoutStyle { };
		std::unique_ptr<Layout> m_layout { };

		std::vector<std::unique_ptr<Widget>> m_children { };
	};
}