//
// Created by omora on 2018-04-06.
//

#ifndef BIRCH_TREE_UTILS_NODES_COUNTS_H
#define BIRCH_TREE_UTILS_NODES_COUNTS_H

#ifdef _MSC_VER
#pragma once
#endif

#include "include_basic.h"
#include "class_CFTree.h"

// this functionality serves only for control, to control the tree structure

std::vector<int> cauntNodesCounts(std::shared_ptr<CFTree> tree);

#endif //BIRCH_TREE_UTILS_NODES_COUNTS_H
