#include "pch.h"

#include <TinyUI/Rendering/Renderer.h>

using namespace tinycore;

namespace tinyui {
	static D2D1_RECT_F ToD2DRect(Rect rect) {
		return D2D1_RECT_F { rect.x, rect.y, rect.x + rect.w, rect.y + rect.h };
	}

	static D2D1_COLOR_F ToD2DColor(Color color) {
		return D2D1_COLOR_F { color.r, color.g, color.b, color.a };
	}

	bool Renderer::InitializeText() {
		HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf()));
		return SUCCEEDED(hr);
	}

	void Renderer::ShutdownText() {
		m_dwriteFactory.Reset();
	}

	void Renderer::Begin(ID2D1HwndRenderTarget* renderTarget) {
		m_renderTarget = renderTarget;
		if (m_renderTarget)
			m_renderTarget->BeginDraw();
	}

	void Renderer::End() {
		if (!m_renderTarget)
			return;

		while (m_renderTarget && m_clipDepth > 0)
			PopClip();

		m_renderTarget->EndDraw();
		m_renderTarget = nullptr;
	}

	void Renderer::Clear(Color color) {
		if (!m_renderTarget)
			return;

		m_renderTarget->Clear(ToD2DColor(color));
	}

	void Renderer::PushClip(Rect rect) {
		if (!m_renderTarget)
			return;

		m_renderTarget->PushAxisAlignedClip(ToD2DRect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		
		++m_clipDepth;
	}

	void Renderer::PopClip() {
		if (!m_renderTarget)
			return;

		m_renderTarget->PopAxisAlignedClip();

		--m_clipDepth;
	}

	std::size_t Renderer::GetClipDepth() const {
		return m_clipDepth;
	}

	void Renderer::FillRect(Rect rect, Color color, float radius) {
		if (!m_renderTarget)
			return;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		HRESULT hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), brush.GetAddressOf());
		if (FAILED(hr))
			return;

		if (radius <= 0.f)
			m_renderTarget->FillRectangle(ToD2DRect(rect), brush.Get());
		else {
			D2D1_ROUNDED_RECT roundedRect { ToD2DRect(rect), radius, radius };
			m_renderTarget->FillRoundedRectangle(roundedRect, brush.Get());
		}
	}

	void Renderer::DrawRect(Rect rect, Color color, float thickness, float radius) {
		if (!m_renderTarget)
			return;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		HRESULT hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), brush.GetAddressOf());
		if (FAILED(hr))
			return;

		if (radius <= 0.f)
			m_renderTarget->DrawRectangle(ToD2DRect(rect), brush.Get(), thickness);
		else {
			D2D1_ROUNDED_RECT roundedRect { ToD2DRect(rect), radius, radius };
			m_renderTarget->DrawRoundedRectangle(roundedRect, brush.Get(), thickness);
		}
	}

	void Renderer::DrawLine(Vec2 start, Vec2 end, Color color, float thickness) {
		if (!m_renderTarget)
			return;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		HRESULT hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), brush.GetAddressOf());
		if (FAILED(hr))
			return;

		D2D1_POINT_2F startPoint { start.x, start.y };
		D2D1_POINT_2F endPoint { end.x, end.y };
		m_renderTarget->DrawLine(startPoint, endPoint, brush.Get(), thickness);
	}

	void Renderer::FillCircle(Vec2 center, float radius, Color color) {
		if (!m_renderTarget)
			return;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		HRESULT hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), brush.GetAddressOf());
		if (FAILED(hr))
			return;

		D2D1_ELLIPSE ellipse { { center.x, center.y }, radius, radius };
		m_renderTarget->FillEllipse(ellipse, brush.Get());
	}

	void Renderer::DrawCircle(Vec2 center, float radius, Color color, float thickness) {
		if (!m_renderTarget)
			return;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		HRESULT hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), brush.GetAddressOf());
		if (FAILED(hr))
			return;

		D2D1_ELLIPSE ellipse { { center.x, center.y }, radius, radius };
		m_renderTarget->DrawEllipse(ellipse, brush.Get(), thickness);
	}

	void Renderer::DrawTextBox(std::wstring_view text, Rect rect, Color color, float fontSize, TextAlign align, TextWrap wrap) {
		if (!m_renderTarget || !m_dwriteFactory || text.empty())
			return;

		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
		HRESULT hr = m_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ko-kr", textFormat.GetAddressOf());
		if (FAILED(hr))
			return;

		textFormat->SetWordWrapping(wrap == TextWrap::NoWrap ? DWRITE_WORD_WRAPPING_NO_WRAP : DWRITE_WORD_WRAPPING_WRAP);

		switch (align) {
			case tinyui::TextAlign::Left:
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			case tinyui::TextAlign::Center:
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			case tinyui::TextAlign::Right:
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
		}

		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), brush.GetAddressOf());
		if (FAILED(hr))
			return;

		m_renderTarget->DrawTextW(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), ToD2DRect(rect), brush.Get());
	}

	Size Renderer::MeasureText(std::wstring_view text, float fontSize) {
		if (!m_dwriteFactory || text.empty())
			return { };

		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
		HRESULT hr = m_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ko-kr", textFormat.GetAddressOf());
		if (FAILED(hr))
			return { };

		textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
		hr = m_dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), 100000.f, 100000.f, textLayout.GetAddressOf());
		if (FAILED(hr))
			return { };

		DWRITE_TEXT_METRICS metrics { };
		hr = textLayout->GetMetrics(&metrics);
		if (FAILED(hr))
			return { };

		return { metrics.widthIncludingTrailingWhitespace, metrics.height };
	}

	Size Renderer::MeasureTextWrapped(std::wstring_view text, float fontSize, float maxWidth) {
		if (!m_dwriteFactory || text.empty())
			return { };

		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
		HRESULT hr = m_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ko-kr", textFormat.GetAddressOf());
		if (FAILED(hr))
			return { };

		textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

		Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
		hr = m_dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), maxWidth, 100000.f, textLayout.GetAddressOf());
		if (FAILED(hr))
			return { };

		DWRITE_TEXT_METRICS metrics { };
		hr = textLayout->GetMetrics(&metrics);
		if (FAILED(hr))
			return { };

		return { metrics.widthIncludingTrailingWhitespace, metrics.height };
	}

	std::size_t Renderer::HitTestTextPosition(std::wstring_view text, float fontSize, float x) {
		if (!m_dwriteFactory || text.empty() || x <= 0.f)
			return 0;

		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
		HRESULT hr = m_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ko-kr", textFormat.GetAddressOf());
		if (FAILED(hr))
			return 0;

		textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
		hr = m_dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), 100000.f, 100000.f, textLayout.GetAddressOf());
		if (FAILED(hr))
			return 0;

		DWRITE_TEXT_METRICS textMetrics { };
		hr = textLayout->GetMetrics(&textMetrics);
		if (SUCCEEDED(hr)) {
			if (x >= textMetrics.widthIncludingTrailingWhitespace)
				return text.size();
		}

		BOOL isTrailingHit = FALSE;
		BOOL isInside = FALSE;

		DWRITE_HIT_TEST_METRICS hitMetrics { };
		hr = textLayout->HitTestPoint(x, fontSize * 0.5f, &isTrailingHit, &isInside, &hitMetrics);
		if (FAILED(hr))
			return 0;

		std::size_t index = static_cast<std::size_t>(hitMetrics.textPosition);
		if (isTrailingHit)
			index += static_cast<std::size_t>(hitMetrics.length);

		if (index > text.size())
			index = text.size();

		return index;
	}
}