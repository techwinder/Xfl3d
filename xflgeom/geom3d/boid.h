/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois
    License: GPL v3

*****************************************************************************/

#pragma once

#include <xflgeom/geom3d/vector3d.h>

struct Boid
{
    int Index{-1};
    Vector3d m_Position;
    Vector3d m_Velocity;
};

