/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QVector>
#include <xflgeom/geom3d/triangle3d.h>
#include <xflgeom/geom3d/node.h>



class Triangulation
{
    public:
        Triangulation();

        Triangle3d const & triangleAt(int idx) const {return m_Triangle.at(idx);}
        Triangle3d &triangle(int idx) {return m_Triangle[idx];}

        void setTriangles(QVector<Triangle3d> const& trianglelist) {m_Triangle=trianglelist;}
        void setTriangle(int it, Triangle3d const &t3d) {if(it<0 || it>=m_Triangle.size()) return; else m_Triangle[it]=t3d;}
        void appendTriangle(Triangle3d const& triangle) {m_Triangle.append(triangle);}
        void appendTriangles(QVector<Triangle3d> const& trianglelist) {m_Triangle.append(trianglelist);}

        void makeXZsymmetric();
        void clearConnections();
        void makeTriangleConnections();
        int makeNodes();
        void makeNodeNormals(bool bReversed=false);

        int isTriangleNode(const Node &nd) const;

        void translate(const Vector3d &T);
        void rotate(const Vector3d &Origin, const Vector3d &axis, double theta);
        void scale(double XFactor, double YFactor, double ZFactor);
        void flipXZ();

        bool areNeighbours(Triangle3d const &t1, Triangle3d const &t2) const;

        void clear() {m_Triangle.clear();  m_Node.clear();}
        int nTriangles() const {return m_Triangle.size();}
        void setTriangleCount(int ntriangles) {m_Triangle.resize(ntriangles);}

        bool intersect(const Vector3d &A, const Vector3d &B, Vector3d &Inear, Vector3d &N) const;

        QVector<Triangle3d> &triangles() {return m_Triangle;}
        QVector<Triangle3d> const &triangles() const {return m_Triangle;}

        QVector<Node> &nodes() {return m_Node;}
        QVector<Node> const &nodes() const{return m_Node;}
        int nNodes()    const {return m_Node.size();}
        int nodeCount() const {return m_Node.size();}
        void setNodeCount(int nnodes) {m_Node.resize(nnodes);}

        void clearNodes() {m_Node.clear();}
        void setNodes(QVector<Node> const &nodes) {m_Node=nodes;}
        void setNode(int in, Node const &nd) {if(in<0||in>nodeCount()) return; else m_Node[in]=nd;}
        void appendNode(Node const &node) {m_Node.append(node);}
        void appendNodes(QVector<Node> const &nodes) {m_Node.append(nodes);}
        Node const & nodeAt(int inode) const {return m_Node.at(inode);}
        Node & node(int inode) {return m_Node[inode];}

        double wettedArea() const;
        void computeSurfaceProperties(double &lx, double &ly, double &lz, double &wettedarea);

        void getFreeEdges(QVector<Segment3d> &freeedges) const;

        void flipNormals();

        bool serializeFl5(QDataStream &ar, bool bIsStoring);

    private:
        QVector<Triangle3d> m_Triangle;
        QVector<Node> m_Node;
};




