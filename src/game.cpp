#include "pch.h"
#include "game.h"
#include "looper.hpp"

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

    if (int r = xx::ReadAllBytes("res/a.dds", gLooper.fileData)) {
        return r;
    }

    return gLooper.Run();
}

void Game::Render() {
    ClearView(DirectX::Colors::MidnightBlue);

    auto secs = xx::NowEpochSeconds();
    for (int i = 0; i < 100; ++i) {
        ComPtr<ID3D11ShaderResourceView> tex;
        auto r = DirectX::CreateDDSTextureFromMemory(d3dDevice.Get(), fileData.buf, fileData.len, nullptr, tex.GetAddressOf());
        //auto r = DirectX::CreateDDSTextureFromFile(d3dDevice.Get(), L"res\\a.dds", nullptr, tex.GetAddressOf());
        assert(r >= 0);
    }
    xx::CoutN("load a.dds 100 times. secs = ", xx::NowEpochSeconds(secs));

    Render1();
    Render2();
    Render3();
}


void Game::Render1() {
    ShaderSwitch(shader_Triangles);
    for (auto& o : vertices1) {
        *shader_Triangles.Alloc(1) = o;
    }
}


xx::Task<> Game::Render2Logic_() {
    using namespace DirectX;
    cb.world = XMMatrixIdentity();
    cb.view = XMMatrixTranspose(XMMatrixLookAtLH(
        XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    ));
    cb.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, float(wndWidth) / wndHeight, 1.0f, 1000.0f));

    double phi{}, theta{};
    while (true) {
        phi += 0.3 * deltaSecs, theta += 0.36 * deltaSecs;
        cb.world = XMMatrixTranspose(XMMatrixRotationX((float)phi) * XMMatrixRotationY((float)theta));
        co_yield 0;
    }
}


void Game::Render2() {
    Render2Logic();
    ShaderSwitch(shader_IndexTriangles);
    auto [vs, is] = shader_IndexTriangles.Alloc(&cb, (UINT)std::size(vertices2), (UINT)std::size(indices2));
    memcpy(vs, vertices2, sizeof(vertices2));
    memcpy(is, indices2, sizeof(indices2));
}


void Game::Render3() {
#if !ENABLE_SCENE_PERFORMANCE_TEST
    if (!scene.gameOver) {
        scene.Update((float)deltaSecs);
    }
#else
    static constexpr float frameDelaySeconds{ 1.f / 60 };

    auto secs = xx::NowEpochSeconds();
    size_t counter{};
    for (size_t i = 0; i < 100000; i++) {
        RobotSimulate::Scene scene;
        while (!scene.gameOver) {
            scene.Update(frameDelaySeconds);
            ++counter;
        }
    }
    secs = xx::NowEpochSeconds(secs);
    xx::CoutN("counter = ", counter, " secs = ", secs, " eps = ", (counter / secs));
#endif
} 
