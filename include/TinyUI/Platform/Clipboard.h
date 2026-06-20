#pragma once

#include <Windows.h>
#include <string>
#include <string_view>

namespace tinyui {
	bool SetClipboardText(std::wstring_view text);
	bool GetClipboardText(std::wstring& outText);
	bool HasClipboardText();
}