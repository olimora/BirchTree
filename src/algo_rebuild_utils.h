//
// Created by omora on 2018-04-09.
//

#ifndef BIRCH_TREE_ALGO_REBUILD_UTILS_H
#define BIRCH_TREE_ALGO_REBUILD_UTILS_H

#ifdef _MSC_VER
#pragma once
#endif

#include "include_basic.h"
#include "class_CFTree.h"

using Path = std::vector<int>;

/**
 * Is path p1 before path p2?
 * @param p1
 * @param p2
 * @return bool
 */
//bool isBefore(Path p1, Path p2);
void calculateNewThresholdT();
int findNodesClosestEntry(std::vector<std::shared_ptr<BNode>> ents, Vec cf_entry);
std::shared_ptr<CF> findNodesClosestEntry(std::vector<std::shared_ptr<CF>> ents, Vec cf_entry);

class Rebuilder {
private:
    Rebuilder() {};
    Rebuilder(const Rebuilder&);
    Rebuilder& operator=(const Rebuilder&);

public:
    static Rebuilder& get()
    {
        static Rebuilder instance;
        return instance;
    }

    std::shared_ptr<CFTree> tree;

    Path old_current_path;
    Path new_current_path;
    Path new_closest_path;
    
    void resetPaths() {
        this->old_current_path.clear();
        this->new_current_path.clear();
        this->new_closest_path.clear();
    }

    std::shared_ptr<CFTree>& getTree() { return this->tree; }
    void makeNewTree() {
        this->tree.reset();
        this->tree = std::make_shared<CFTree>();
    }
};

#endif //BIRCH_TREE_ALGO_REBUILD_UTILS_H
