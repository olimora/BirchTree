//
// Created by omora on 2018-03-17.
//

#ifndef BIRCH_TREE_CLASS_CF_H
#define BIRCH_TREE_CLASS_CF_H

#ifdef _MSC_VER
#pragma once
#endif


#include "include_basic.h"


class CF : public std::enable_shared_from_this<CF> {
public:
    int N; // number of points
    Vec LS; // linear sum by dimensions
    double SS; // squared sum

    CF();
    CF(int N, Vec LS, double SS);
    CF(std::shared_ptr<CF> cf1, std::shared_ptr<CF> cf2);

    Vec getCentroid();

    double getClusterSize();
    double getClusterSize(Vec point);

    void addPoint(Vec point);

    int getMemoryConsumed();
};

#endif //BIRCH_TREE_CLASS_CF_H
