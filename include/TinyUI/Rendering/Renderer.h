#pragma once

#include <TinyCore/Core/Types.h>

#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <cstddef>

#include <vector>
#include <string>
#include <string_view>

using namespace tinycore;

namespace tinyui {
	enum class TextAlign { Left, Center, Right };

	enum class TextWrap { NoWrap, Wrap };

	class Renderer final {
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
		void DrawLine(Vec2 start, Vec2 end, Color color, float thickness = 1.f);
		void FillCircle(Vec2 center, float radius, Color color);
		void DrawCircle(Vec2 center, float radius, Color color, float thickness = 1.f);

		void DrawTextBox(std::wstring_view text, Rect rect, Color color, float fontSize = 14.f, TextAlign align = TextAlign::Left, TextWrap wrap = TextWrap::NoWrap);
		Size MeasureText(std::wstring_view text, float fontSize = 14.f, TextWrap wrap = TextWrap::NoWrap);
		Size MeasureText(std::wstring_view text, tinycore::Size maxSize, float fontSize = 14.f, TextWrap wrap = TextWrap::NoWrap);

		std::size_t HitTestTextIndex(std::wstring_view text, tinycore::Size layoutSize, float fontSize, TextWrap wrap, tinycore::Vec2 localPosition);
		tinycore::Vec2 GetTextCaretPosition(std::wstring_view text, std::size_t textIndex, tinycore::Size layoutSize, float fontSize, TextWrap wrap);
		std::vector<tinycore::Rect> HitTestTextRange(std::wstring_view text, std::size_t startIndex, std::size_t length, tinycore::Size layoutSize, float fontSize, TextWrap wrap);

	private:
		ID2D1HwndRenderTarget* m_renderTarget = nullptr;

		std::wstring m_fontFamily = L"Segoe UI";

		Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory;

		std::size_t m_clipDepth = 0;
	};
}