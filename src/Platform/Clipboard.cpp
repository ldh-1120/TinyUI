#include <TinyUI/Platform/Clipboard.h>

#include <Windows.h>
#include <cstring>

namespace tinyui {
	bool SetClipboardText(std::wstring_view text) {
		if (!OpenClipboard(nullptr))
			return false;

		if (!EmptyClipboard()) {
			CloseClipboard();
			return false;
		}

		const std::size_t characterCount = text.size() + 1;
		const SIZE_T byteCount = static_cast<SIZE_T>(characterCount * sizeof(wchar_t));
		HGLOBAL memoryHandle = GlobalAlloc(GMEM_MOVEABLE, byteCount);
		if (!memoryHandle) {
			CloseClipboard();
			return false;
		}

		void* memory = GlobalLock(memoryHandle);
		if (!memory) {
			GlobalFree(memoryHandle);
			CloseClipboard();
			return false;
		}

		std::memcpy(memory, text.data(), text.size() * sizeof(wchar_t));

		wchar_t* characters = static_cast<wchar_t*>(memory);
		characters[text.size()] = L'\0';

		GlobalUnlock(memoryHandle);
		if (!SetClipboardData(CF_UNICODETEXT, memoryHandle)) {
			GlobalFree(memoryHandle);
			CloseClipboard();
			return false;
		}

		CloseClipboard();

		return true;
	}

	bool GetClipboardText(std::wstring& outText) {
		outText.clear();

		if (!IsClipboardFormatAvailable(CF_UNICODETEXT) || !OpenClipboard(nullptr))
			return false;

		HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
		if (!clipboardData) {
			CloseClipboard();
			return false;
		}

		const wchar_t* text = static_cast<const wchar_t*>(GlobalLock(clipboardData));
		if (!text) {
			CloseClipboard();
			return false;
		}

		outText = text;

		GlobalUnlock(clipboardData);
		CloseClipboard();

		return true;
	}

	bool HasClipboardText() {
		return IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE;
	}
}