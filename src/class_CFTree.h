//
// Created by omora on 2018-03-24.
//

#ifndef BIRCH_TREE_CLASS_CFTREE_H
#define BIRCH_TREE_CLASS_CFTREE_H

#ifdef _MSC_VER
#pragma once
#endif

#include "include_basic.h"
#include "class_Node.h"


class CFTree {
public:
    std::shared_ptr<NLNode> root;
    int NLNode_count;
    int LNode_count;
    int subcluster_count;

    CFTree();
    int getPointCount();
};

#endif //BIRCH_TREE_CLASS_CFTREE_H
