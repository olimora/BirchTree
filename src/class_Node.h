//
// Created by omora on 2018-03-17.
//


#ifndef BIRCH_TREE_CLASS_NODE_H
#define BIRCH_TREE_CLASS_NODE_H

#ifdef _MSC_VER
#pragma once
#endif


#include "include_basic.h"
#include "class_CF.h"

class BNode {
public:
    std::shared_ptr<CF> cf;

    BNode() {}
    virtual bool isLeafNode() = 0;
    virtual int getBF() = 0;

    Vec getCentroid();
};

class NLNode : public BNode {
public:
    std::vector<std::shared_ptr<BNode>> entries;

    NLNode();

    bool isLeafNode() { return false; }
    int getBF();

};

class LNode : public BNode {
public:
    std::vector<std::shared_ptr<CF>> entries;

    LNode();
    LNode(Vec point);

    bool isLeafNode() { return true; }
    int getBF();

};



#endif //BIRCH_TREE_C_NODE_H
