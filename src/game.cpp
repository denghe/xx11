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


void Game::Render2() {
    timePool += deltaSecs;
    while (timePool >= frameDelaySecs) {
        timePool -= frameDelaySecs;

        // todo: update logic
    }

    ShaderSwitch(shader_IndexTriangles);
    auto [vs, is] = shader_IndexTriangles.Alloc(std::size(vertices2), std::size(indices2));
    memcpy(vs, vertices2, sizeof(vertices2));
    memcpy(is, indices2, sizeof(indices2));
    // todo: set cb
}
