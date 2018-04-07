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
    //virtual std::vector<std::shared_ptr<BEntry>> getEntries() = 0; //TODO: a v derived to budu derived entries

    Vec getCentroid();
};

class NLNode : public BNode {
public:
    std::vector<std::shared_ptr<BNode>> entries;
    //std::vector<std::shared_ptr<NLEntry>> entries;


    NLNode();

    bool isLeafNode() { return false; }
    int getBF();
    //std::vector<std::shared_ptr<BEntry>> getEntries() { return this->entries; } ;

    int getMemoryConsumed();
};

class LNode : public BNode {
public:
    std::vector<std::shared_ptr<CF>> entries;
    //std::vector<std::shared_ptr<LEntry>> entries;

    std::shared_ptr<LNode> left;
    std::shared_ptr<LNode> right;

    LNode();
    LNode(Vec point);

    bool isLeafNode() { return true; }
    int getBF();
    //std::vector<std::shared_ptr<BEntry>> getEntries() { return this->entries; } ;

    int getMemoryConsumed();
};



#endif //BIRCH_TREE_C_NODE_H
