#pragma once
#include "pch.h"

struct Looper {
	Looper() = default;
	Looper(Looper const&) = delete;
	Looper& operator=(Looper const&) = delete;

	// need set before call Init()
	std::wstring className, title;
	int wndWidth{}, wndHeight{};

	// return 0 == success
	int Init(HINSTANCE hInstance, int nCmdShow);
	int Run();

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int InitWindow(HINSTANCE hInstance, int nCmdShow);
	int InitDevice();
	void CleanupDevice();
};
