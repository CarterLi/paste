#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

enum ExitReason
{
	Success,
	NoTextualData,
	ClipboardError,
	SystemError
};

void WriteError(const wchar_t *error)
{
	HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
	if (hErr == INVALID_HANDLE_VALUE || !hErr)
	{
		ExitProcess((UINT)SystemError);
	}
	DWORD charsWritten = -1;
	WriteConsoleW(hErr, error, lstrlenW(error), &charsWritten, 0);
	CloseHandle(hErr);
}

bool ClipboardContainsFormat(UINT format)
{
	bool firstTime = true;
	for (UINT f = 0; firstTime || f != 0; f = EnumClipboardFormats(f))
	{
		firstTime = false;
		if (f == format)
		{
			return true;
		}
	}
	return false;
}

int main()
{
	if (!OpenClipboard(0))
	{
		WriteError(L"Failed to open system clipboard!\n");
		ExitProcess((UINT)ClipboardError);
	}

	if (!ClipboardContainsFormat(CF_UNICODETEXT))
	{
		CloseClipboard();
		WriteError(L"Clipboard contains non-text data!\n");
		ExitProcess((UINT)NoTextualData);
	}

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == INVALID_HANDLE_VALUE || !hData)
	{
		CloseClipboard();
		WriteError(L"Unable to get clipboard data!\n");
		ExitProcess((UINT)ClipboardError);
	}

	const wchar_t *text = (const wchar_t *) GlobalLock(hData);
	if (!text)
	{
		CloseClipboard();
		WriteError(L"Unable to get clipboard data!\n");
		ExitProcess((UINT)ClipboardError);
	}

	DWORD charsWritten = -1;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleW(hOut, text, lstrlenW(text), &charsWritten, 0);
	CloseHandle(hOut);

	GlobalUnlock(hData);
	CloseClipboard();

	ExitProcess((UINT)Success);
}
