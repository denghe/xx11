﻿#include "pch.h"
#include "looper.h"

Looper gLooper;

int Looper::InitWindow(HINSTANCE hInstance, int nCmdShow) {
    if (className.empty())
        return __LINE__;

    if (title.empty())
        return __LINE__;

    WNDCLASSEX c{};
    c.cbSize = sizeof(c);
    c.style = CS_HREDRAW | CS_VREDRAW;      // redraw when H V size changed
    c.lpfnWndProc = Looper::WndProc;
    c.hInstance = hInstance;
    c.lpszClassName = className.c_str();
    //c.cbClsExtra = 0;
    //c.cbWndExtra = 0;
    //c.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //c.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //c.lpszMenuName = nullptr;
    //c.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    //c.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);

    if (!RegisterClassEx(&c)) {
        auto e = GetLastError();
        return __LINE__;
    }

    RECT rc = { 0, 0, wndWidth, wndHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // this step will call WndProc 3 times
    hWnd = CreateWindow(className.c_str(), title.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        auto e = GetLastError();
        return __LINE__;
    }

    ShowWindow(hWnd, nCmdShow);
    return 0;
}


void Looper::ShowConsole() {
    ::AllocConsole();
    FILE* tmp{};
    freopen_s(&tmp, "CONIN$", "r", stdin);
    freopen_s(&tmp, "CONOUT$", "w", stdout);
    freopen_s(&tmp, "CONOUT$", "w", stderr);
    ::SetConsoleOutputCP(CP_UTF8);

    auto ConfigConsoleMode = [](DWORD id) {
        auto handle = ::GetStdHandle(id);
        if (handle) {
            DWORD mode{};
            ::GetConsoleMode(handle, &mode);
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            ::SetConsoleMode(handle, mode);
        }
        };
    ConfigConsoleMode(STD_OUTPUT_HANDLE);
    ConfigConsoleMode(STD_ERROR_HANDLE);
}


int Looper::InitDevice() {
    HRESULT hr{ S_OK };

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
        driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &d3dDevice, &featureLevel, &immediateContext);

        if (hr == E_INVALIDARG) {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &d3dDevice, &featureLevel, &immediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return __LINE__;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    ComPtr<IDXGIFactory1> dxgiFactory;
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        hr = d3dDevice.As(&dxgiDevice);
        if (SUCCEEDED(hr)) {
            ComPtr<IDXGIAdapter> adapter;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr)) {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1)
                    , reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
            }
        }
    }
    if (FAILED(hr))
        return __LINE__;

    // Create swap chain
    ComPtr<IDXGIFactory2> dxgiFactory2;
    hr = dxgiFactory.As(&dxgiFactory2);
    if (dxgiFactory2) {
        // DirectX 11.1 or later
        hr = d3dDevice.As(&d3dDevice1);
        if (SUCCEEDED(hr)) {
            (void)immediateContext.As(&immediateContext1);
        }

        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, &swapChain1);
        if (SUCCEEDED(hr)) {
            hr = swapChain1.As(&swapChain);
        }

    } else {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(d3dDevice.Get(), &sd, &swapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    dxgiFactory.Reset();

    if (FAILED(hr))
        return __LINE__;

    // Create a render target view
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D)
        , reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return __LINE__;

    hr = d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
    if (FAILED(hr))
        return __LINE__;

    immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    immediateContext->RSSetViewports(1, &vp);

    return 0;
}


/*
https://wiki.winehq.org/List_Of_Windows_Messages
*/
void Looper::InitMessageTexts() {
    assert(messageTexts.empty());
#define REGISTER_MESSAGE_TEXT(msg) messageTexts[msg] = #msg;
    REGISTER_MESSAGE_TEXT(WM_NULL);
    REGISTER_MESSAGE_TEXT(WM_CREATE);
    REGISTER_MESSAGE_TEXT(WM_DESTROY);
    REGISTER_MESSAGE_TEXT(WM_MOVE);
    REGISTER_MESSAGE_TEXT(WM_SIZE);
    REGISTER_MESSAGE_TEXT(WM_ACTIVATE);
    REGISTER_MESSAGE_TEXT(WM_SETFOCUS);
    REGISTER_MESSAGE_TEXT(WM_KILLFOCUS);
    REGISTER_MESSAGE_TEXT(WM_ENABLE);
    REGISTER_MESSAGE_TEXT(WM_SETREDRAW);
    REGISTER_MESSAGE_TEXT(WM_SETTEXT);
    REGISTER_MESSAGE_TEXT(WM_GETTEXT);
    REGISTER_MESSAGE_TEXT(WM_GETTEXTLENGTH);
    REGISTER_MESSAGE_TEXT(WM_PAINT);
    REGISTER_MESSAGE_TEXT(WM_CLOSE);
    REGISTER_MESSAGE_TEXT(WM_QUERYENDSESSION);
    REGISTER_MESSAGE_TEXT(WM_QUIT);
    REGISTER_MESSAGE_TEXT(WM_QUERYOPEN);
    REGISTER_MESSAGE_TEXT(WM_ERASEBKGND);
    REGISTER_MESSAGE_TEXT(WM_SYSCOLORCHANGE);
    REGISTER_MESSAGE_TEXT(WM_ENDSESSION);
    REGISTER_MESSAGE_TEXT(WM_SHOWWINDOW);
    REGISTER_MESSAGE_TEXT(WM_WININICHANGE);
    REGISTER_MESSAGE_TEXT(WM_DEVMODECHANGE);
    REGISTER_MESSAGE_TEXT(WM_ACTIVATEAPP);
    REGISTER_MESSAGE_TEXT(WM_FONTCHANGE);
    REGISTER_MESSAGE_TEXT(WM_TIMECHANGE);
    REGISTER_MESSAGE_TEXT(WM_CANCELMODE);
    REGISTER_MESSAGE_TEXT(WM_SETCURSOR);
    REGISTER_MESSAGE_TEXT(WM_MOUSEACTIVATE);
    REGISTER_MESSAGE_TEXT(WM_CHILDACTIVATE);
    REGISTER_MESSAGE_TEXT(WM_QUEUESYNC);
    REGISTER_MESSAGE_TEXT(WM_GETMINMAXINFO);
    REGISTER_MESSAGE_TEXT(WM_PAINTICON);
    REGISTER_MESSAGE_TEXT(WM_ICONERASEBKGND);
    REGISTER_MESSAGE_TEXT(WM_NEXTDLGCTL);
    REGISTER_MESSAGE_TEXT(WM_SPOOLERSTATUS);
    REGISTER_MESSAGE_TEXT(WM_DRAWITEM);
    REGISTER_MESSAGE_TEXT(WM_MEASUREITEM);
    REGISTER_MESSAGE_TEXT(WM_DELETEITEM);
    REGISTER_MESSAGE_TEXT(WM_VKEYTOITEM);
    REGISTER_MESSAGE_TEXT(WM_CHARTOITEM);
    REGISTER_MESSAGE_TEXT(WM_SETFONT);
    REGISTER_MESSAGE_TEXT(WM_GETFONT);
    REGISTER_MESSAGE_TEXT(WM_SETHOTKEY);
    REGISTER_MESSAGE_TEXT(WM_GETHOTKEY);
    REGISTER_MESSAGE_TEXT(WM_QUERYDRAGICON);
    REGISTER_MESSAGE_TEXT(WM_COMPAREITEM);
    REGISTER_MESSAGE_TEXT(WM_GETOBJECT);
    REGISTER_MESSAGE_TEXT(WM_COMPACTING);
    REGISTER_MESSAGE_TEXT(WM_COMMNOTIFY);
    REGISTER_MESSAGE_TEXT(WM_WINDOWPOSCHANGING);
    REGISTER_MESSAGE_TEXT(WM_WINDOWPOSCHANGED);
    REGISTER_MESSAGE_TEXT(WM_POWER);
    REGISTER_MESSAGE_TEXT(WM_COPYDATA);
    REGISTER_MESSAGE_TEXT(WM_CANCELJOURNAL);
    REGISTER_MESSAGE_TEXT(WM_NOTIFY);
    REGISTER_MESSAGE_TEXT(WM_INPUTLANGCHANGEREQUEST);
    REGISTER_MESSAGE_TEXT(WM_INPUTLANGCHANGE);
    REGISTER_MESSAGE_TEXT(WM_TCARD);
    REGISTER_MESSAGE_TEXT(WM_HELP);
    REGISTER_MESSAGE_TEXT(WM_USERCHANGED);
    REGISTER_MESSAGE_TEXT(WM_NOTIFYFORMAT);
    REGISTER_MESSAGE_TEXT(WM_CONTEXTMENU);
    REGISTER_MESSAGE_TEXT(WM_STYLECHANGING);
    REGISTER_MESSAGE_TEXT(WM_STYLECHANGED);
    REGISTER_MESSAGE_TEXT(WM_DISPLAYCHANGE);
    REGISTER_MESSAGE_TEXT(WM_GETICON);
    REGISTER_MESSAGE_TEXT(WM_SETICON);
    REGISTER_MESSAGE_TEXT(WM_NCCREATE);
    REGISTER_MESSAGE_TEXT(WM_NCDESTROY);
    REGISTER_MESSAGE_TEXT(WM_NCCALCSIZE);
    REGISTER_MESSAGE_TEXT(WM_NCHITTEST);
    REGISTER_MESSAGE_TEXT(WM_NCPAINT);
    REGISTER_MESSAGE_TEXT(WM_NCACTIVATE);
    REGISTER_MESSAGE_TEXT(WM_GETDLGCODE);
    REGISTER_MESSAGE_TEXT(WM_SYNCPAINT);
    REGISTER_MESSAGE_TEXT(WM_NCMOUSEMOVE);
    REGISTER_MESSAGE_TEXT(WM_NCLBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_NCLBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_NCLBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_NCRBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_NCRBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_NCRBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_NCMBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_NCMBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_NCMBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_NCXBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_NCXBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_NCXBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(EM_GETSEL);
    REGISTER_MESSAGE_TEXT(EM_SETSEL);
    REGISTER_MESSAGE_TEXT(EM_GETRECT);
    REGISTER_MESSAGE_TEXT(EM_SETRECT);
    REGISTER_MESSAGE_TEXT(EM_SETRECTNP);
    REGISTER_MESSAGE_TEXT(EM_SCROLL);
    REGISTER_MESSAGE_TEXT(EM_LINESCROLL);
    REGISTER_MESSAGE_TEXT(EM_SCROLLCARET);
    REGISTER_MESSAGE_TEXT(EM_GETMODIFY);
    REGISTER_MESSAGE_TEXT(EM_SETMODIFY);
    REGISTER_MESSAGE_TEXT(EM_GETLINECOUNT);
    REGISTER_MESSAGE_TEXT(EM_LINEINDEX);
    REGISTER_MESSAGE_TEXT(EM_SETHANDLE);
    REGISTER_MESSAGE_TEXT(EM_GETHANDLE);
    REGISTER_MESSAGE_TEXT(EM_GETTHUMB);
    REGISTER_MESSAGE_TEXT(EM_LINELENGTH);
    REGISTER_MESSAGE_TEXT(EM_REPLACESEL);
    REGISTER_MESSAGE_TEXT(EM_GETLINE);
    REGISTER_MESSAGE_TEXT(EM_LIMITTEXT);
    REGISTER_MESSAGE_TEXT(EM_SETLIMITTEXT);
    REGISTER_MESSAGE_TEXT(EM_CANUNDO);
    REGISTER_MESSAGE_TEXT(EM_UNDO);
    REGISTER_MESSAGE_TEXT(EM_FMTLINES);
    REGISTER_MESSAGE_TEXT(EM_LINEFROMCHAR);
    REGISTER_MESSAGE_TEXT(EM_SETTABSTOPS);
    REGISTER_MESSAGE_TEXT(EM_SETPASSWORDCHAR);
    REGISTER_MESSAGE_TEXT(EM_EMPTYUNDOBUFFER);
    REGISTER_MESSAGE_TEXT(EM_GETFIRSTVISIBLELINE);
    REGISTER_MESSAGE_TEXT(EM_SETREADONLY);
    REGISTER_MESSAGE_TEXT(EM_SETWORDBREAKPROC);
    REGISTER_MESSAGE_TEXT(EM_GETWORDBREAKPROC);
    REGISTER_MESSAGE_TEXT(EM_GETPASSWORDCHAR);
    REGISTER_MESSAGE_TEXT(EM_SETMARGINS);
    REGISTER_MESSAGE_TEXT(EM_GETMARGINS);
    REGISTER_MESSAGE_TEXT(EM_GETLIMITTEXT);
    REGISTER_MESSAGE_TEXT(EM_POSFROMCHAR);
    REGISTER_MESSAGE_TEXT(EM_CHARFROMPOS);
    REGISTER_MESSAGE_TEXT(EM_SETIMESTATUS);
    REGISTER_MESSAGE_TEXT(EM_GETIMESTATUS);
    REGISTER_MESSAGE_TEXT(SBM_SETPOS);
    REGISTER_MESSAGE_TEXT(SBM_GETPOS);
    REGISTER_MESSAGE_TEXT(SBM_SETRANGE);
    REGISTER_MESSAGE_TEXT(SBM_GETRANGE);
    REGISTER_MESSAGE_TEXT(SBM_ENABLE_ARROWS);
    REGISTER_MESSAGE_TEXT(SBM_SETRANGEREDRAW);
    REGISTER_MESSAGE_TEXT(SBM_SETSCROLLINFO);
    REGISTER_MESSAGE_TEXT(SBM_GETSCROLLINFO);
    REGISTER_MESSAGE_TEXT(SBM_GETSCROLLBARINFO);
    REGISTER_MESSAGE_TEXT(BM_GETCHECK);
    REGISTER_MESSAGE_TEXT(BM_SETCHECK);
    REGISTER_MESSAGE_TEXT(BM_GETSTATE);
    REGISTER_MESSAGE_TEXT(BM_SETSTATE);
    REGISTER_MESSAGE_TEXT(BM_SETSTYLE);
    REGISTER_MESSAGE_TEXT(BM_CLICK);
    REGISTER_MESSAGE_TEXT(BM_GETIMAGE);
    REGISTER_MESSAGE_TEXT(BM_SETIMAGE);
    REGISTER_MESSAGE_TEXT(BM_SETDONTCLICK);
    REGISTER_MESSAGE_TEXT(WM_INPUT);
    REGISTER_MESSAGE_TEXT(WM_KEYDOWN);
    REGISTER_MESSAGE_TEXT(WM_KEYFIRST);
    REGISTER_MESSAGE_TEXT(WM_KEYUP);
    REGISTER_MESSAGE_TEXT(WM_CHAR);
    REGISTER_MESSAGE_TEXT(WM_DEADCHAR);
    REGISTER_MESSAGE_TEXT(WM_SYSKEYDOWN);
    REGISTER_MESSAGE_TEXT(WM_SYSKEYUP);
    REGISTER_MESSAGE_TEXT(WM_SYSCHAR);
    REGISTER_MESSAGE_TEXT(WM_SYSDEADCHAR);
    REGISTER_MESSAGE_TEXT(WM_KEYLAST);
    REGISTER_MESSAGE_TEXT(WM_UNICHAR);
    REGISTER_MESSAGE_TEXT(WM_IME_STARTCOMPOSITION);
    REGISTER_MESSAGE_TEXT(WM_IME_ENDCOMPOSITION);
    REGISTER_MESSAGE_TEXT(WM_IME_COMPOSITION);
    REGISTER_MESSAGE_TEXT(WM_IME_KEYLAST);
    REGISTER_MESSAGE_TEXT(WM_INITDIALOG);
    REGISTER_MESSAGE_TEXT(WM_COMMAND);
    REGISTER_MESSAGE_TEXT(WM_SYSCOMMAND);
    REGISTER_MESSAGE_TEXT(WM_TIMER);
    REGISTER_MESSAGE_TEXT(WM_HSCROLL);
    REGISTER_MESSAGE_TEXT(WM_VSCROLL);
    REGISTER_MESSAGE_TEXT(WM_INITMENU);
    REGISTER_MESSAGE_TEXT(WM_INITMENUPOPUP);
    REGISTER_MESSAGE_TEXT(WM_MENUSELECT);
    REGISTER_MESSAGE_TEXT(WM_MENUCHAR);
    REGISTER_MESSAGE_TEXT(WM_ENTERIDLE);
    REGISTER_MESSAGE_TEXT(WM_MENURBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_MENUDRAG);
    REGISTER_MESSAGE_TEXT(WM_MENUGETOBJECT);
    REGISTER_MESSAGE_TEXT(WM_UNINITMENUPOPUP);
    REGISTER_MESSAGE_TEXT(WM_MENUCOMMAND);
    REGISTER_MESSAGE_TEXT(WM_CHANGEUISTATE);
    REGISTER_MESSAGE_TEXT(WM_UPDATEUISTATE);
    REGISTER_MESSAGE_TEXT(WM_QUERYUISTATE);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLORMSGBOX);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLOREDIT);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLORLISTBOX);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLORBTN);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLORDLG);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLORSCROLLBAR);
    REGISTER_MESSAGE_TEXT(WM_CTLCOLORSTATIC);
    REGISTER_MESSAGE_TEXT(WM_MOUSEFIRST);
    REGISTER_MESSAGE_TEXT(WM_MOUSEMOVE);
    REGISTER_MESSAGE_TEXT(WM_LBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_LBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_LBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_RBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_RBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_RBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_MBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_MBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_MBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_MOUSELAST);
    REGISTER_MESSAGE_TEXT(WM_MOUSEWHEEL);
    REGISTER_MESSAGE_TEXT(WM_XBUTTONDOWN);
    REGISTER_MESSAGE_TEXT(WM_XBUTTONUP);
    REGISTER_MESSAGE_TEXT(WM_XBUTTONDBLCLK);
    REGISTER_MESSAGE_TEXT(WM_PARENTNOTIFY);
    REGISTER_MESSAGE_TEXT(WM_ENTERMENULOOP);
    REGISTER_MESSAGE_TEXT(WM_EXITMENULOOP);
    REGISTER_MESSAGE_TEXT(WM_NEXTMENU);
    REGISTER_MESSAGE_TEXT(WM_SIZING);
    REGISTER_MESSAGE_TEXT(WM_CAPTURECHANGED);
    REGISTER_MESSAGE_TEXT(WM_MOVING);
    REGISTER_MESSAGE_TEXT(WM_POWERBROADCAST);
    REGISTER_MESSAGE_TEXT(WM_DEVICECHANGE);
    REGISTER_MESSAGE_TEXT(WM_MDICREATE);
    REGISTER_MESSAGE_TEXT(WM_MDIDESTROY);
    REGISTER_MESSAGE_TEXT(WM_MDIACTIVATE);
    REGISTER_MESSAGE_TEXT(WM_MDIRESTORE);
    REGISTER_MESSAGE_TEXT(WM_MDINEXT);
    REGISTER_MESSAGE_TEXT(WM_MDIMAXIMIZE);
    REGISTER_MESSAGE_TEXT(WM_MDITILE);
    REGISTER_MESSAGE_TEXT(WM_MDICASCADE);
    REGISTER_MESSAGE_TEXT(WM_MDIICONARRANGE);
    REGISTER_MESSAGE_TEXT(WM_MDIGETACTIVE);
    REGISTER_MESSAGE_TEXT(WM_MDISETMENU);
    REGISTER_MESSAGE_TEXT(WM_ENTERSIZEMOVE);
    REGISTER_MESSAGE_TEXT(WM_EXITSIZEMOVE);
    REGISTER_MESSAGE_TEXT(WM_DROPFILES);
    REGISTER_MESSAGE_TEXT(WM_MDIREFRESHMENU);
    REGISTER_MESSAGE_TEXT(WM_IME_SETCONTEXT);
    REGISTER_MESSAGE_TEXT(WM_IME_NOTIFY);
    REGISTER_MESSAGE_TEXT(WM_IME_CONTROL);
    REGISTER_MESSAGE_TEXT(WM_IME_COMPOSITIONFULL);
    REGISTER_MESSAGE_TEXT(WM_IME_SELECT);
    REGISTER_MESSAGE_TEXT(WM_IME_CHAR);
    REGISTER_MESSAGE_TEXT(WM_IME_REQUEST);
    REGISTER_MESSAGE_TEXT(WM_IME_KEYDOWN);
    REGISTER_MESSAGE_TEXT(WM_IME_KEYUP);
    REGISTER_MESSAGE_TEXT(WM_NCMOUSEHOVER);
    REGISTER_MESSAGE_TEXT(WM_MOUSEHOVER);
    REGISTER_MESSAGE_TEXT(WM_NCMOUSELEAVE);
    REGISTER_MESSAGE_TEXT(WM_MOUSELEAVE);
    REGISTER_MESSAGE_TEXT(WM_CUT);
    REGISTER_MESSAGE_TEXT(WM_COPY);
    REGISTER_MESSAGE_TEXT(WM_PASTE);
    REGISTER_MESSAGE_TEXT(WM_CLEAR);
    REGISTER_MESSAGE_TEXT(WM_UNDO);
    REGISTER_MESSAGE_TEXT(WM_RENDERFORMAT);
    REGISTER_MESSAGE_TEXT(WM_RENDERALLFORMATS);
    REGISTER_MESSAGE_TEXT(WM_DESTROYCLIPBOARD);
    REGISTER_MESSAGE_TEXT(WM_DRAWCLIPBOARD);
    REGISTER_MESSAGE_TEXT(WM_PAINTCLIPBOARD);
    REGISTER_MESSAGE_TEXT(WM_VSCROLLCLIPBOARD);
    REGISTER_MESSAGE_TEXT(WM_SIZECLIPBOARD);
    REGISTER_MESSAGE_TEXT(WM_ASKCBFORMATNAME);
    REGISTER_MESSAGE_TEXT(WM_CHANGECBCHAIN);
    REGISTER_MESSAGE_TEXT(WM_HSCROLLCLIPBOARD);
    REGISTER_MESSAGE_TEXT(WM_QUERYNEWPALETTE);
    REGISTER_MESSAGE_TEXT(WM_PALETTEISCHANGING);
    REGISTER_MESSAGE_TEXT(WM_PALETTECHANGED);
    REGISTER_MESSAGE_TEXT(WM_HOTKEY);
    REGISTER_MESSAGE_TEXT(WM_PRINT);
    REGISTER_MESSAGE_TEXT(WM_PRINTCLIENT);
    REGISTER_MESSAGE_TEXT(WM_APPCOMMAND);
    REGISTER_MESSAGE_TEXT(WM_DWMNCRENDERINGCHANGED);
    REGISTER_MESSAGE_TEXT(WM_HANDHELDFIRST);
    REGISTER_MESSAGE_TEXT(WM_HANDHELDLAST);
    REGISTER_MESSAGE_TEXT(WM_AFXFIRST);
    REGISTER_MESSAGE_TEXT(WM_AFXLAST);
    REGISTER_MESSAGE_TEXT(WM_PENWINFIRST);
    REGISTER_MESSAGE_TEXT(WM_PENWINLAST);
    REGISTER_MESSAGE_TEXT(WM_USER);
#undef REGISTER_MESSAGE_TEXT
}


void Looper::DumpMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    std::string s;
    if (auto iter = messageTexts.find(message); iter != messageTexts.end()) {
        xx::Append(s, "message = ", iter->second, " wParam = ", wParam, " lParam = ", lParam, '\n');
    } else {
        if (message > 0x0400) {
            s.append("RESERVED message = ");
        } else {
            s.append("UNKNOWN message = ");
        }
        xx::Append(s,message, '\n');
    }
    printf("%s", s.c_str());
}
