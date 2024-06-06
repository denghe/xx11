#include "pch.h"
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
    c.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //c.cbClsExtra = 0;
    //c.cbWndExtra = 0;
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


void Looper::RenderBegin() {
    shader = &shader_Triangles;
}


void Looper::ClearView(FLOAT const* color) {
    immediateContext->ClearRenderTargetView(renderTargetView.Get(), color);
    //immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void Looper::RenderEnd() {
    ShaderCommit();

    auto hr = swapChain->Present(0, 0);
    if (FAILED(hr)) {
        assert(false);
    }
}
