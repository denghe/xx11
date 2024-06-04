#pragma once
#include "pch.h"
#include "shader.h"


struct Shader_Test1 : Shader {

    int Init() override;
    void Begin() override;
    void End() override;
    void Commit();
    // todo: Draw?
};
