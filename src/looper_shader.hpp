#pragma once

template<typename Derived>
int Looper<Derived>::InitShaders() {
    if (int r = shader_Triangles.Init()) return r;
    if (int r = shader_IndexTriangles.Init()) return r;
    // ...
    if (int r = shader_QuadInstance.Init()) return r;
    // ...
    return 0;
}


template<typename Derived>
void Looper<Derived>::ShaderSwitch(Shader& s) {
    if (shader != &s) {
        shader->Commit();
    }
    shader = &s;
}


template<typename Derived>
void Looper<Derived>::ShaderCommit() {
    if (shader) {
        shader->Commit();
        shader = {};
    }
}
