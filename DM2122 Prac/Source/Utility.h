#ifndef UTILITY_H
#define UTILITY_H

#include "Mtx44.h"
#include "Vertex.h"

Position operator*(const Mtx44& lhs, const Position& rhs);
float distancecalculator(Vector3 a, Vector3 b);
#endif
