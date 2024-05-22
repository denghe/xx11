#pragma once
#include "pch.h"

struct Looper {
	Looper() = default;
	Looper(Looper const&) = delete;
	Looper& operator=(Looper const&) = delete;
	~Looper();

	// need set before call Init()
	std::wstring className, title;
	int wndWidth{}, wndHeight{};		// not include title, border, ...

	// return 0 == success
	int Init(HINSTANCE hInstance, int nCmdShow);
	int Run();

protected:
	inline static std::unordered_map<UINT, std::wstring> messageTexts;
	static void DumpMessage(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	int InitWindow(HINSTANCE hInstance, int nCmdShow);
	int InitDevice();
	void CleanupDevice();
	void InitMessageTexts();
};
