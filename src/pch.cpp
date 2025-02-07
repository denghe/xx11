#include "pch.h"
#include "game.h"
#include <mimalloc-new-delete.h>

Game gLooper;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    gLooper.className = L"xx2d_dx11_engine_main_window";
    gLooper.title = L"xx2d engine ( dx11 )";
    gLooper.wndWidth = 1280;
    gLooper.wndHeight = 720;

    if (int r = gLooper.Init(hInstance, nCmdShow, true))
        return 0;

    return gLooper.Run();
}
