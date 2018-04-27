//
// Created by omora on 2018-04-09.
//

#include "algo_rebuild_utils.h"
#include "algo_distance.h"
#include "include_global.h"
#include "algo_utils.h"
#include <cmath>

void recursiveThresholdCall(std::shared_ptr<BNode> node);
void calculateNewThresholdT() {
    Global::get().trhs_dist_count = 0;
    Global::get().trhs_dist_sum = 0.;

    // RECURSIVE CALL - when it gets in the leaf node - ged closest distance, add to global::dist_sum
    recursiveThresholdCall(Global::get().getTree()->root);

    // then dist_sum divide by Global::tree->subclusters
    double computed_T = Global::get().trhs_dist_sum / Global::get().trhs_dist_count;// Global::get().dist_count;
    double increase = std::abs((computed_T - Global::get().getThreshold_T())) / Global::get().getRebuildSizeFactor();
    double new_T = Global::get().getThreshold_T() + increase;

//    std::cout << "Old T = " << Global::get().getThreshold_T()
//              << ", computed_T = " << computed_T
//              << ", increase = " << increase
//              << ", new_T = " << new_T
//              << ", Diff = " << new_T - Global::get().getThreshold_T() << std::endl;

    // set new thr
    Global::get().setThreshold_T(new_T);
}

void recursiveThresholdCall(std::shared_ptr<BNode> node) {
    if (node->isLeafNode()) {
        auto lnode = std::dynamic_pointer_cast<LNode>(node);
        double dist = getClosestDistanceInEntries(lnode->entries);
        if (dist > 0) {
            Global::get().trhs_dist_sum += getClosestDistanceInEntries(lnode->entries);
            Global::get().trhs_dist_count++;
        }
    } else {
        auto nlnode = std::dynamic_pointer_cast<NLNode>(node);
        for (std::shared_ptr<BNode> entry : nlnode->entries) {
            recursiveThresholdCall(entry);
        }
    }
}

int findNodesClosestEntry(std::vector<std::shared_ptr<BNode>> ents, Vec cf_entry) {
    int closest_ent = 0;
    double closest_dist = distance(ents[closest_ent]->getCentroid(), cf_entry);
    for (unsigned int i = 1; i < ents.size(); i++) {   // iterate through the rest and save new closest dist and reference to entry
        double act_dist = distance(ents[i]->getCentroid(), cf_entry);
            if (act_dist < closest_dist) {
            closest_dist = act_dist;
            closest_ent = i;
        }
    }
    return closest_ent;
};

std::shared_ptr<CF> findNodesClosestEntry(std::vector<std::shared_ptr<CF>> ents, Vec cf_entry) {
    int closest_ent = 0;
    double closest_dist = distance(ents[closest_ent]->getCentroid(), cf_entry);
    for (unsigned int i = 1; i < ents.size(); i++) {   // iterate through the rest and save new closest dist and reference to entry
        double act_dist = distance(ents[i]->getCentroid(), cf_entry);
        if (act_dist < closest_dist) {
            closest_dist = act_dist;
            closest_ent = i;
        }
    }
    return ents[closest_ent];
};