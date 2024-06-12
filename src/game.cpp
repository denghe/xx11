#include "pch.h"
#include "game.h"

void Game::Render() {
    ClearView(DirectX::Colors::MidnightBlue);
    //Render1();
    Render2();
}


void Game::Render1() {
    ShaderSwitch(shader_Triangles);
    for (auto& o : vertices1) {
        *shader_Triangles.Alloc(1) = o;
    }
}


xx::Task<> Game::Render2Logic_() {
    using namespace DirectX;
    auto& cb = shader_IndexTriangles.constantBuffer;
    cb.world = XMMatrixIdentity();
    cb.view = XMMatrixTranspose(XMMatrixLookAtLH(
        XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    ));
    cb.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, float(wndWidth) / wndHeight, 1.0f, 1000.0f));

    float phi{}, theta{};
    while (true) {
        phi += 0.001f * deltaSecs, theta += 0.0012f * deltaSecs;
        cb.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta));
        co_yield 0;
    }
}


void Game::Render2() {
    ShaderSwitch(shader_IndexTriangles);
    Render2Logic();
    auto [vs, is] = shader_IndexTriangles.Alloc(std::size(vertices2), std::size(indices2));
    memcpy(vs, vertices2, sizeof(vertices2));
    memcpy(is, indices2, sizeof(indices2));
}
