// Copyright © 2021 Audulus LLC. All rights reserved.

#ifndef commands_h
#define commands_h

#include "include/vger_types.h"

#ifdef __METAL_VERSION__
#define DEVICE device
#else
#define DEVICE

#include <simd/simd.h>
using namespace simd;

#endif

// Rendering commands for experimental tile-based fine rendering.
// Not yet in use.

#define tileBufSize 64
#define maxTilesWidth 256
#define tileSize 16

enum vgerOp {
    vgerOpEnd,
    vgerOpLine,
    vgerOpBez,
    vgerOpSolid,
    vgerOpSegment,
};

/// Line segment.
struct vgerCmdSegment {
    vgerOp op;
    float2 a;
    float2 b;
    float width;
};

/// Flip the sign of the df if ray intersects with line.
struct vgerCmdLineFill {
    vgerOp op;
    float2 a;
    float2 b;
};

/// Flip the sign of the df if point is inside the bezier.
struct vgerCmdBezFill {
    vgerOp op;
    float2 a;
    float2 b;
    float2 c;
};

/// Set the color.
struct vgerCmdSolid {
    vgerOp op;
    int color;
};

struct Tile {
    uint length;
    char commands[tileBufSize];

    template<class T>
    void append(const T cmd) DEVICE {
        *(DEVICE T*) (commands + length) = cmd;
        length += sizeof(T);
    }

    void segment(float2 a, float2 b, float width) DEVICE {
        append(vgerCmdSegment{vgerOpSegment, a, b, width});
    }

    void lineFill(float2 a, float2 b) DEVICE {
        append(vgerCmdLineFill{vgerOpLine, a, b});
    }

    void bezFill(float2 a, float2 b, float2 c) DEVICE {
        append(vgerCmdBezFill{vgerOpBez, a, b, c});
    }

    void solid(int color) DEVICE {
        append(vgerCmdSolid{vgerOpSolid, color});
    }

    void end() DEVICE {
        append(vgerOpEnd);
    }
};

#endif /* commands_h */
