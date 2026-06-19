#pragma once

#include <TinyUI/Core/Types.h>

#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

#include <string_view>

namespace tinyui {
	enum class TextAlign { Left, Center, Right };

	class Renderer {
	public:
		Renderer() = default;

		bool InitializeText();
		void ShutdownText();

		void Begin(ID2D1HwndRenderTarget* renderTarget);
		void End();

		void Clear(Color color);

		void FillRect(Rect rect, Color color, float radius = 0.f);
		void DrawRect(Rect rect, Color color, float thickness = 1.f, float radius = 0.f);

		void DrawTextBox(std::wstring_view text, Rect rect, Color color, float fontSize = 14.f, TextAlign align = TextAlign::Left);

	private:
		ID2D1HwndRenderTarget* m_renderTarget = nullptr;

		Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory;
	};
}