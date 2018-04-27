//
// Created by omora on 2018-03-26.
//

#ifndef BIRCH_TREE_ALGO_DISTANCE_H
#define BIRCH_TREE_ALGO_DISTANCE_H

#ifdef _MSC_VER
#pragma once
#endif

#include "utils_vector.h"

typedef double (*distanceFPtr)(Vec A, Vec B);

double distance(Vec A, Vec B);

distanceFPtr assignDistanceMetric(std::string metric);

double dist_D0(Vec A, Vec B);

double dist_D1(Vec A, Vec B);

// TODO:: other distance metrics



#endif //BIRCH_TREE_ALGO_DISTANCE_H
