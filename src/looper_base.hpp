#pragma once

template<typename Derived>
int Looper<Derived>::Init(HINSTANCE hInstance, int nCmdShow, bool showConsole) {
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

    lastSecs = xx::NowEpochSeconds();
    deltaSecs = 0.00001;

    return 0;
}


template<typename Derived>
Looper<Derived>::~Looper() {
    // todo: clear others contain devices
    if (immediateContext) immediateContext->ClearState();
}


template<typename Derived>
LRESULT CALLBACK Looper<Derived>::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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


template<typename Derived>
int Looper<Derived>::Run() {
    MSG msg{};

#if 1
    bool stoped{};
    std::thread t{ [&] {
        while (!stoped) {
            RenderBegin();
            ((Derived*)this)->Render();
            RenderEnd();
        }
    } };

    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    stoped = true;
    t.join();
#else
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            RenderBegin();
            ((Derived*)this)->Render();
            RenderEnd();
        }
    }
#endif

    return (int)msg.wParam;
}
