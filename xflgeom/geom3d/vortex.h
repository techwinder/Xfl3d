/****************************************************************************

    flow5 application
    Copyright (C) Andre Deperrois 
    All rights reserved.

*****************************************************************************/


#pragma once

#include <xflgeom/geom3d/segment3d.h>
#include <xflcore/enums_objects.h>


void vortexInducedVelocity(Vector3d const &A, Vector3d const &B, Vector3d const &C, Vector3d &vel, double coreradius, xfl::enumVortex vortexmodel=xfl::POTENTIAL) ;
double damp(double r, xfl::enumVortex vortexmodel, double coreradius);


class Vortex : public Segment3d
{
    public:
        Vortex();
        Vortex(Node const &vtx0, Node const &vtx1, double gamma=0);

        void setCirculation(double gamma) {m_Circulation=gamma;}
        double circulation() const {return m_Circulation;}

        bool serializeFl5(QDataStream &ar, bool bIsStoring);

        void getInducedVelocity(Vector3d const &C, Vector3d &vel, double coreradius, xfl::enumVortex vortexmodel=xfl::POTENTIAL) const;

        static void setCoreRadius(double CoreRadius) {s_CoreRadius=CoreRadius;}
        static double coreRadius() {return s_CoreRadius; }

        static xfl::enumVortex vortexModel() {return s_VortexModel;}
        static void setVortexModel(xfl::enumVortex vtxmodel) {s_VortexModel=vtxmodel;}


    private:
        double m_Circulation;

        static xfl::enumVortex s_VortexModel;
        static double s_CoreRadius;
};
