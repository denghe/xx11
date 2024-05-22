#pragma execution_character_set("utf-8")
#pragma once
#ifndef _XX11_H_
#define _XX11_H_

#include <xx_task.h>
#include <d3d11_1.h>
#include <directxcolors.h>

struct Looper;
extern Looper					g_looper;

extern HWND						g_hWnd;
extern D3D_DRIVER_TYPE			g_driverType;
extern D3D_FEATURE_LEVEL		g_featureLevel;
extern ID3D11Device*			g_pd3dDevice;
extern ID3D11Device1*			g_pd3dDevice1;
extern ID3D11DeviceContext*		g_pImmediateContext;
extern ID3D11DeviceContext1*	g_pImmediateContext1;
extern IDXGISwapChain*			g_pSwapChain;
extern IDXGISwapChain1*			g_pSwapChain1;
extern ID3D11RenderTargetView*	g_pRenderTargetView;

#endif
