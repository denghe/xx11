#pragma once
#include "looper.h"

struct Game : Looper<Game> {

    void Render();

    // simple trangle
    static constexpr Shader_Triangles::Vert vertices1[]{
        { {0.0f, 0.1f, 0.1f}, {0.0f, 1.0f, 0.0f, 1.0f} },
        { {0.1f, -0.1f, 0.1f}, {0.0f, 0.0f, 1.0f, 1.0f} },
        { {-0.1f, -0.1f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f} }
    };

    void Render1(); // draw simple trangle


    //    5________ 6
    //    /|      /|
    //   /_|_____/ |
    //  1|4|_ _ 2|_|7
    //   | /     | /
    //   |/______|/
    //  0       3
    static constexpr Shader_IndexTriangles::Vert vertices2[]{
        { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f} },
        { {-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
        { {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
        { {1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
        { {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
        { {-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
        { {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
        { {1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} }
    };

    static constexpr UINT indices2[]{
        // front
        0, 1, 2,
        2, 3, 0,
        // left
        4, 5, 1,
        1, 0, 4,
        // top
        1, 5, 6,
        6, 2, 1,
        // rear
        7, 6, 5,
        5, 4, 7,
        // right
        3, 2, 6,
        6, 7, 3,
        // bottom
        4, 0, 3,
        3, 7, 4
    };

    static constexpr double frameDelaySecs{ 1. / 60. };
    double timePool{};
    void Render2(); // draw rotate 3d cube
};
