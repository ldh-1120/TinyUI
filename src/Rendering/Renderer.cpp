#include <TinyUI/Rendering/Renderer.h>

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
	}

	void Renderer::End() {
		m_renderTarget = nullptr;
	}

	void Renderer::Clear(Color color) {
		if (!m_renderTarget)
			return;

		m_renderTarget->Clear(ToD2DColor(color));
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

	void Renderer::DrawTextBox(std::wstring_view text, Rect rect, Color color, float fontSize, TextAlign align) {
		if (!m_renderTarget || !m_dwriteFactory || text.empty())
			return;

		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;

		HRESULT hr = m_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ko-kr", textFormat.GetAddressOf());
		if (FAILED(hr))
			return;

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
}