#include "pch.h"
#include "looper.h"

void Looper::Render() {
    // Just clear the backbuffer
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::MidnightBlue);
    swapChain->Present(0, 0);
}
