#include "pch.h"
#include "looper.h"


int Looper::Init(HINSTANCE hInstance, int nCmdShow, bool showConsole =
#ifndef NDEBUG
    true
#else
    false
#endif
) {
    if (showConsole) {
        ShowConsole();
    }
    if (wndWidth <= 0)
        return __LINE__;
    if (wndHeight <= 0)
        return __LINE__;
    // ...
    InitMessageTexts();
    // ...
    if (int r = InitWindow(hInstance, nCmdShow))
        return r;
    if (int r = InitDevice())
        return r;
    // ...
    InitShaders();
    // ...
    return 0;
}


Looper::~Looper() {
    // todo: clear others contain devices
    if (immediateContext) immediateContext->ClearState();
}


LRESULT CALLBACK Looper::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

#ifndef NDEBUG
    DumpMessage(message, wParam, lParam);
#endif

    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break; 

    case WM_DESTROY:
        PostQuitMessage(__LINE__);
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        XX_ASSUME(false);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


int Looper::Run() {
    MSG msg{};

    bool stoped{};
    std::thread t{ [&] {
        while (!stoped) {
            Render();
        }
    } };

    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } // else Render();
    }

    stoped = true;
    t.join();

    return (int)msg.wParam;
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    gLooper.className = L"xx2d_dx11_engine_main_window";
    gLooper.title = L"xx2d engine ( dx11 )";
    gLooper.wndWidth = 1280;
    gLooper.wndHeight = 720;

    if (int r = gLooper.Init(hInstance, nCmdShow))
        return 0;

    return gLooper.Run();
}
