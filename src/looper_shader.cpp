#include "pch.h"
#include "looper.h"

int Looper::InitShaders() {

    if (int r = shader_QuadInstanceData.Init()) return r;
    // ...

    return 0;
}
