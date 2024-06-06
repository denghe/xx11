#include "pch.h"
#include "looper.h"

static Shader_Triangles::Buf vertices[]{
    { {0.0f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} },
    { {0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
    { {-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} }
};

void Looper::Render() {
    ClearView(DirectX::Colors::MidnightBlue);

    ShaderSwitch(shader_Triangles);
    for (auto& o : vertices) {
        auto buf = shader_Triangles.Alloc(1);
        *buf = o;
    }
}
