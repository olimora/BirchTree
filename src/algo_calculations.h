#ifndef ALGO_CALCULATIONS_H
#define ALGO_CALCULATIONS_H

#ifdef _MSC_VER
#pragma once
#endif  

////////////////////////////////////////

#include "include_basic.h"
#include "class_CF.h"

typedef double (*clusterSizeFPtr)(std::shared_ptr<CF> cf, Vec point);

double calculateClusterSize(std::shared_ptr<CF> cf, Vec point);

clusterSizeFPtr assignClusterSizeMetric(std::string metric);

Vec calculateCentroid(Vec LS, int N);

double calculateRadius(std::shared_ptr<CF> cf, Vec point);

//double calculateRadius(int N, Vec LS, double SS, Vec C); // C is centroid

double calculateDiameter(std::shared_ptr<CF> cf, Vec point);

//double calculateDiameter(int N, Vec LS, double SS);

#endif
