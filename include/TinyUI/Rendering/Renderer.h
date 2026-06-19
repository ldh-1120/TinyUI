#pragma once

#include <TinyUI/Core/Types.h>

#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <cstddef>

#include <string_view>

namespace tinyui {
	enum class TextAlign { Left, Center, Right };

	enum class TextWrap { NoWrap, Wrap };

	class Renderer {
	public:
		Renderer() = default;

		bool InitializeText();
		void ShutdownText();

		void Begin(ID2D1HwndRenderTarget* renderTarget);
		void End();

		void Clear(Color color);

		void PushClip(Rect rect);
		void PopClip();

		std::size_t GetClipDepth() const;

		void FillRect(Rect rect, Color color, float radius = 0.f);
		void DrawRect(Rect rect, Color color, float thickness = 1.f, float radius = 0.f);

		void DrawTextBox(std::wstring_view text, Rect rect, Color color, float fontSize = 14.f, TextAlign align = TextAlign::Left, TextWrap wrap = TextWrap::NoWrap);
		Size MeasureText(std::wstring_view text, float fontSize = 14.f);

		std::size_t HitTestTextPosition(std::wstring_view text, float fontSize, float x);

	private:
		ID2D1HwndRenderTarget* m_renderTarget = nullptr;

		Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory;

		std::size_t m_clipDepth = 0;
	};
}