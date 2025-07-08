/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

namespace xfl
{
    /** @enum The different types of polar available for 2D and 3D calculations. */
    enum enumPolarType {T1POLAR, T2POLAR, T3POLAR, T4POLAR, T5POLAR, T6POLAR, T7POLAR, T8POLAR, BOATPOLAR, EXTERNALPOLAR};


    /**
    * @enum enumeration used to identify the type of surface on which the panel lies.
    * May be on a bottom, mid, top, side, or body surface.
    */
    enum enumSurfacePosition {BOTSURFACE, MIDSURFACE, TOPSURFACE, SIDESURFACE, FUSESURFACE, WAKESURFACE, NOSURFACE};

    /** @enum The different models for the vortex viscous core */
    typedef enum {POTENTIAL, CUT_OFF, LAMB_OSEEN, RANKINE, SCULLY, VATISTAS} enumVortex;


}




