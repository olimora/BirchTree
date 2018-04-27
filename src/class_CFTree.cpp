//
// Created by omora on 2018-03-24.
//

#include "class_CFTree.h"
#include "algo_build_tree.h"

CFTree::CFTree() {
    this->root = std::make_shared<NLNode>();
    this->NLNode_count = 1;
    this->LNode_count = 0;
    this->subcluster_count = 0;
}

int CFTree::getPointCount() {
    return this->root->cf->N;
}

