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

    InitMessageTexts();

    if (int r = InitWindow(hInstance, nCmdShow))
        return r;

    if (int r = InitDevice())
        return r;

    InitShaders();

    return 0;
}


template<typename Derived>
Looper<Derived>::~Looper() {
    // todo: clear others contain devices
    if (immediateContext) immediateContext->ClearState();
}

static constexpr UINT const MESSAGE_UPDATE = WM_USER + 12345;

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

    /******************************************************************************************************/
    // Keeping things moving during Win32 Move/Resize events
    // https://gamedev.net/forums/topic/672094-keeping-things-moving-during-win32-moveresize-events/5254386/
    case WM_NCLBUTTONDOWN:
        if (SendMessage(hWnd, WM_NCHITTEST, wParam, lParam) == HTCAPTION) {
            POINT point;
            GetCursorPos(&point);
            ScreenToClient(hWnd, &point);
            PostMessage(hWnd, WM_MOUSEMOVE, 0, point.x | point.y << 16);
        }
        break;
    case WM_ENTERSIZEMOVE:
        SetTimer(hWnd, 1, 0, NULL);
        break;
    case WM_EXITSIZEMOVE:
        KillTimer(hWnd, 1);
        break;
    case WM_TIMER:
        gLooper.Frame();
        break;
    /******************************************************************************************************/

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

template<typename Derived>
XX_INLINE void Looper<Derived>::Frame() {
    FrameBegin();
    RenderBegin();
    ((Derived*)this)->Render();
    RenderEnd();
    FrameEnd();
}

template<typename Derived>
int Looper<Derived>::Run() {
    MSG msg{};

    BeforeRun();

    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Frame();
    }

    return (int)msg.wParam;
}
