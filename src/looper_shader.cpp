#include "pch.h"
#include "looper.h"

int Looper::InitShaders() {
    if (int r = shader_QuadInstance.Init()) return r;
    if (int r = shader_Triangles.Init()) return r;
    // ...
    return 0;
}


XX_FORCE_INLINE void Looper::ShaderSwitch(Shader& s) {
    if (shader != &s) {
        shader->Commit();
    }
    shader = &s;
}


XX_FORCE_INLINE void Looper::ShaderCommit() {
    if (shader) {
        shader->Commit();
        shader = {};
    }
}
