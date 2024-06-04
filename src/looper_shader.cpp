#include "pch.h"
#include "looper.h"

int Looper::InitShaders() {

    if (int r = shader_QuadInstance.Init()) return r;
    if (int r = shader_Test1.Init()) return r;
    // ...

    return 0;
}
