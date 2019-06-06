
//
// Intersection methods
//

#pragma once

#include "CGImport3.h"
#include <CoreStructures\CoreStructures.h>


CGIMPORT3_API void calculateBarycentricCoordinates(const CoreStructures::GUVector2& a, const CoreStructures::GUVector2& b, const CoreStructures::GUVector2& c, const CoreStructures::GUVector2& p, float *u, float *v, float *w);
CGIMPORT3_API CoreStructures::GUVector2 createPointFromBarycentricCoords(const CoreStructures::GUVector2& a, const CoreStructures::GUVector2& b, const CoreStructures::GUVector2& c, const float u, const float v, const float w);
CGIMPORT3_API CoreStructures::GUVector4 createVectorFromBarycentricCoords(CoreStructures::GUVector4 *a, CoreStructures::GUVector4 *b, CoreStructures::GUVector4 *c, const float u, const float v, const float w);
CGIMPORT3_API float cross2D(const CoreStructures::GUVector2& u, const CoreStructures::GUVector2& v);
CGIMPORT3_API bool pointInProjectedFace(const CoreStructures::GUVector2& p, const CoreStructures::GUVector2& a, const CoreStructures::GUVector2& b, const CoreStructures::GUVector2& c);
CGIMPORT3_API float signed2DTriArea(const CoreStructures::GUVector2& a, const CoreStructures::GUVector2& b, const CoreStructures::GUVector2& c);
CGIMPORT3_API bool intersect2DLineSegment(const CoreStructures::GUVector2& a, const CoreStructures::GUVector2& b, const CoreStructures::GUVector2& c, const CoreStructures::GUVector2& d, float *t, CoreStructures::GUVector2 *p);
CGIMPORT3_API bool rayTriangleIntersect(CoreStructures::GUVector4 *p1, CoreStructures::GUVector4 *p2, CoreStructures::GUVector4 *v0, CoreStructures::GUVector4 *v1, CoreStructures::GUVector4 *v2, double *t, double *u, double *v);

