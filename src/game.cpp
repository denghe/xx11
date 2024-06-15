#include "pch.h"
#include "game.h"

void Game::Render() {
    ClearView(DirectX::Colors::MidnightBlue);
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
    auto [vs, is] = shader_IndexTriangles.Alloc(&cb, std::size(vertices2), std::size(indices2));
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
