#include "FileDialogue.h"

#include <commdlg.h>

CommonItemDialogue::CommonItemDialogue(HWND ownerWindow_)
	: ownerWindow(ownerWindow_)
{
}

bool CommonItemDialogue::Open(std::wstring& filePath, std::wstring& errorMessage)
{
	wchar_t fileName[MAX_PATH] = {};
	OPENFILENAMEW dialog = {};
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = ownerWindow;
	dialog.lpstrFile = fileName;
	dialog.nMaxFile = ARRAYSIZE(fileName);
	dialog.lpstrFilter = L"RAW volume files (*.raw)\0*.raw\0All files (*.*)\0*.*\0";
	dialog.nFilterIndex = 1;
	dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	dialog.lpstrTitle = L"Open RAW volume";

	if (GetOpenFileNameW(&dialog) == TRUE)
	{
		filePath = fileName;
		return true;
	}

	if (CommDlgExtendedError() != 0)
	{
		errorMessage = L"The Windows file dialog could not be opened.";
	}

	return false;
}
