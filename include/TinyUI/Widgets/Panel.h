#pragma once

#include <TinyUI/Widgets/Widget.h>

#include <string>
#include <string_view>

namespace tinyui {
	struct PanelOptions {
		bool showTitle = true;

		bool collapsible = false;
		bool defaultExpanded = true;

		float cornerRadius = -1.f;
		float borderThickness = -1.f;
		float titleHeight = -1.f;
		float titleFontSize = -1.f;

		tinycore::Thickness contentPadding { };
		bool useCustomContentPadding = false;
	};

	class Panel final : public Widget {
	public:
		explicit Panel(std::wstring title = L"");

		void SetTitle(std::wstring_view title);
		const std::wstring& GetTitle() const;

		void SetOptions(const PanelOptions& options);
		const PanelOptions& GetOptions() const;

		Rect GetContentRect() const override;

		bool IsCollapsible() const;
		bool IsExpanded() const;
		void SetExpanded(bool expanded);
		void ToggleExpanded();

		bool IsMouseInteractive() const override;

	protected:
		void OnPaint(PaintContext& context) override;

		void OnMouseDown(MouseEvent& event) override;

		bool ShouldPaintChildren() const override;
		bool ShouldArrangeChildren() const override;
		bool ShouldHitTestChildren() const override;

		tinycore::Size MeasureOverride(tinycore::Size availableSize) override;

	private:
		float GetCornerRadius(const Theme& theme) const;
		float GetBorderThickness(const Theme& theme) const;
		float GetTitleHeight(const Theme& theme) const;
		float GetTitleFontSize(const Theme& theme) const;
		tinycore::Thickness GetContentPadding(const Theme& theme) const;

		bool IsInTitleArea(tinycore::Vec2 position) const;
		float GetResolvedTitleHeight(const Theme& theme) const;

	private:
		std::wstring m_title { };
		PanelOptions m_options { };

		bool m_collapsible = false;
		bool m_expanded = true;
		bool m_hasInitializedExpanded = false;
	};
}