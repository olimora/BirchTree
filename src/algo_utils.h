//
// Created by omora on 2018-03-27.
//

#ifndef BIRCH_TREE_ALGO_UTILS_H
#define BIRCH_TREE_ALGO_UTILS_H

#ifdef _MSC_VER
#pragma once
#endif

#include "include_basic.h"
#include "class_Node.h"
#include "class_Entry.h"


std::shared_ptr<BNode> findNodesClosestEntry(std::shared_ptr<NLNode> node, Vec point);
std::shared_ptr<CF> findNodesClosestEntry(std::shared_ptr<LNode> node, Vec point);

std::vector<std::shared_ptr<BNode>> findClosestEntriesPair(std::vector<std::shared_ptr<BNode>> ents);

std::vector<std::shared_ptr<BNode>> findFurthestEntries(std::vector<std::shared_ptr<BNode>> ents);
std::vector<std::shared_ptr<CF>> findFurthestEntries(std::vector<std::shared_ptr<CF>> ents);


#endif //BIRCH_TREE_ALGO_UTILS_H
