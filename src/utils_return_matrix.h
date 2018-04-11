//
// Created by omora on 2018-04-11.
//

#ifndef BIRCH_TREE_UTILS_RETURN_MATRIX_H
#define BIRCH_TREE_UTILS_RETURN_MATRIX_H

#ifdef _MSC_VER
#pragma once
#endif

#include "include_basic.h"
#include "class_CFTree.h"

int getRowsCount();
int getColsCount();

int getIndexInMatrix(int row, int col, int rows);
void fillReturnMatrixWithSubclusters(std::shared_ptr<CFTree> tree, double* matrix);

#endif //BIRCH_TREE_UTILS_RETURN_MATRIX_H
