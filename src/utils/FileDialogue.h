#pragma once

#include <Windows.h>
#include <string>

class IFileDialogue
{
public:
	virtual ~IFileDialogue() = default;
	virtual bool Open(std::wstring& filePath, std::wstring& errorMessage) = 0;
};

class CommonItemDialogue final : public IFileDialogue
{
public:
	explicit CommonItemDialogue(HWND ownerWindow);
	bool Open(std::wstring& filePath, std::wstring& errorMessage) override;

private:
	HWND ownerWindow;
};
