/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/


#include <xflgraph/graph/graph.h>
class GraphWt;

void exportGraphToSvg(const GraphWt *pGraphWt, Graph const *pGraph, QString &tempfilepath);
void exportGraphDataToFile(Graph const*pGraph);
