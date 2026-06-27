#pragma once

namespace tinyui {
	class UIBuilder;
	class Widget;

	class UIScope final {
	public:
		UIScope() = default;
		explicit UIScope(UIBuilder* builder, Widget* widget);

		UIScope(const UIScope&) = delete;
		UIScope& operator=(const UIScope&) = delete;

		UIScope(UIScope&& other) noexcept;
		UIScope& operator=(UIScope&& other) noexcept;

		~UIScope();

		void End();
		bool IsActive() const;

		Widget& GetWidget() const;

	private:
		UIBuilder* m_builder = nullptr;
		Widget* m_widget = nullptr;
		bool m_active = false;
	};
}