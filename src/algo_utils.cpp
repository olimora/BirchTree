//
// Created by omora on 2018-03-27.
//


#include "algo_utils.h"
#include "algo_distance.h"


//template<class Node_T, class Entry_T>
std::shared_ptr<BNode> findNodesClosestEntry(std::shared_ptr<NLNode> node, Vec point) {
    std::shared_ptr<BNode> closest_ent = node->entries[0];
    double closest_dist = distance(closest_ent->getCentroid(), point);
    for (unsigned int i = 1; i < node->entries.size(); i++) {   // iterate through the rest and save new closest dist and reference to entry
        double act_dist = distance(node->entries[i]->getCentroid(), point);
        if (act_dist < closest_dist) {
            closest_dist = act_dist;
            closest_ent = node->entries[i];
        }
    }
    return closest_ent;
};

std::shared_ptr<CF> findNodesClosestEntry(std::shared_ptr<LNode> node, Vec point) {
    std::shared_ptr<CF> closest_ent = node->entries[0];
    double closest_dist = distance(closest_ent->getCentroid(), point);
    for (unsigned int i = 1; i < node->entries.size(); i++) {   // iterate through the rest and save new closest dist and reference to entry
        double act_dist = distance(node->entries[i]->getCentroid(), point);
        if (act_dist < closest_dist) {
            closest_dist = act_dist;
            closest_ent = node->entries[i];
        }
    }
    return closest_ent;
};

std::vector<std::shared_ptr<BNode>> findClosestEntriesPair(std::vector<std::shared_ptr<BNode>> ents) {
    int A = 0;
    int B = 1;
    double min_dist = distance(ents[A]->getCentroid(), ents[B]->getCentroid());
    double act_dist = 0.;
    for (int i = 2; i < int(ents.size()); i++) { // B
        for (int j = 0; j < i; j++) {   // A
            act_dist = distance(ents[j]->getCentroid(), ents[i]->getCentroid());
            if (act_dist < min_dist) {
                min_dist = act_dist;
                A = j;
                B = i;
            }
        }
    }
    std::vector<std::shared_ptr<BNode>> returning;
    returning.reserve(2);
    returning.push_back(ents[A]);
    returning.push_back(ents[B]);
    return returning;
}

double getClosestDistanceInEntries(std::vector<std::shared_ptr<CF>> ents) {
    if (ents.size() < 2) {
        return 0.;
    }
    double min_dist = distance(ents[0]->getCentroid(), ents[1]->getCentroid());
    double act_dist = 0.;
    if (ents.size() < 3) {
        return min_dist;
    }
    for (int i = 2; i < int(ents.size()); i++) { // B
        for (int j = 0; j < i; j++) {   // A
            act_dist = distance(ents[j]->getCentroid(), ents[i]->getCentroid());
            if (act_dist < min_dist) {
                min_dist = act_dist;
            }
        }
    }
    return min_dist;
}

std::vector<std::shared_ptr<CF>> findFurthestEntries(std::vector<std::shared_ptr<CF>> ents) {
    int A = 0;
    int B = 1;
    double max_dist = distance(ents[A]->getCentroid(), ents[B]->getCentroid());
    double act_dist = 0.;
    for (int i = 2; i < int(ents.size()); i++) { // B
        for (int j = 0; j < i; j++) {   // A
            act_dist = distance(ents[j]->getCentroid(), ents[i]->getCentroid());
            if (act_dist > max_dist) {
                max_dist = act_dist;
                A = j;
                B = i;
            }
        }
    }
    std::vector<std::shared_ptr<CF>> returning;
    returning.reserve(2);
    returning.push_back(ents[A]);
    returning.push_back(ents[B]);
    return returning;
}

std::vector<std::shared_ptr<BNode>> findFurthestEntries(std::vector<std::shared_ptr<BNode>> ents) {
    int A = 0;
    int B = 1;
    double max_dist = distance(ents[A]->getCentroid(), ents[B]->getCentroid());
    double act_dist = 0.;
    for (int i = 2; i < int(ents.size()); i++) { // B
        for (int j = 0; j < i; j++) {   // A
            act_dist = distance(ents[j]->getCentroid(), ents[i]->getCentroid());
            if (act_dist > max_dist) {
                max_dist = act_dist;
                A = j;
                B = i;
            }
        }
    }
    std::vector<std::shared_ptr<BNode>> returning;
    returning.reserve(2);
    returning.push_back(ents[A]);
    returning.push_back(ents[B]);
    return returning;
}