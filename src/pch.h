#pragma execution_character_set("utf-8")
#pragma once
#ifndef _XX11_H_
#define _XX11_H_

#include <xx_task.h>
#include <d3d11_1.h>
#include <directxcolors.h>
#include <wrl/client.h>		// ComPtr

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Looper;
extern Looper					gLooper;

#endif
