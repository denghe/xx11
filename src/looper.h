﻿#pragma once
#include "pch.h"
#include "shader.h"

struct Looper {
protected:
	friend Shader;

	HWND hWnd{};
	D3D_DRIVER_TYPE driverType{ D3D_DRIVER_TYPE_NULL };
	D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_0 };
	ComPtr<ID3D11Device> d3dDevice;
	ComPtr<ID3D11Device1> d3dDevice1;
	ComPtr<ID3D11DeviceContext> immediateContext;
	ComPtr<ID3D11DeviceContext1> immediateContext1;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<IDXGISwapChain1> swapChain1;
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	Shader_QuadInstanceData shader_QuadInstanceData;

public:
	Looper() = default;
	Looper(Looper const&) = delete;
	Looper& operator=(Looper const&) = delete;
	~Looper();

	// need set before call Init()
	std::wstring className, title;
	int wndWidth{}, wndHeight{};		// not include title, border, ...

	// return 0 == success
	int Init(HINSTANCE hInstance, int nCmdShow, bool showConsole);
	int Run();

protected:
	inline static std::unordered_map<UINT, std::string> messageTexts;
	static void InitMessageTexts();
	static void DumpMessage(UINT message, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void ShowConsole();

	int InitWindow(HINSTANCE hInstance, int nCmdShow);
	int InitDevice();
	int InitShaders();
};
