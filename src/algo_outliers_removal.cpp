//
// Created by omora on 2018-04-13.
//

#include "algo_outliers_removal.h"
#include "algo_rebuild_tree.h"
#include "include_global.h"
#include "algo_utils.h"

double min_density;
int outliers_removed_subclusters;
int outliers_removed_points;

void findCandidates(std::shared_ptr<BNode> node, std::vector<std::shared_ptr<CF>>& candidates);
bool isCandidate(std::shared_ptr<CF>& cf);
void tryInsertCandidates(std::vector<std::shared_ptr<CF>>& candidates);
bool insertCandidate(std::shared_ptr<BNode> node, std::shared_ptr<CF> candidate);

void removeOutliers() {
    std::cout << "/// Outlier Removal ///////////////////////////////////////////" << std::endl;
    // rebuild tree with the same threshold_T, to merge leaf entries, that are not outleirs
    std::cout << "Rebuilding tree to merge leaf entries." << std::endl;
    rebuildTreeBeforeOutliersRemoval();

    std::cout << "Root N count = " << Global::get().getTree()->root->cf->N << std::endl;

    // go through subclusters and select candidates for subclusters
    min_density = Global::get().getTree()->root->cf->getDensity();
    std::cout << "Root density = " << min_density << std::endl;
    std::vector<std::shared_ptr<CF>> candidates;
    findCandidates(Global::get().getTree()->root, candidates);
    std::cout << "Candidates count = " << candidates.size() << std::endl;
    std::cout << "Root N count = " << Global::get().getTree()->root->cf->N << std::endl;

    // try to insert (absorbe) candidates to tree without growing
    tryInsertCandidates(candidates);
    std::cout << "Candidates count after reinsertion = " << candidates.size() << std::endl;
    std::cout << "Root N count = " << Global::get().getTree()->root->cf->N << std::endl;
}

void findCandidates(std::shared_ptr<BNode> node, std::vector<std::shared_ptr<CF>>& candidates) {
    if (!node->isLeafNode()) { // non leaf
        std::shared_ptr<NLNode> nlnode = std::dynamic_pointer_cast<NLNode>(node);
        // reset this CF for CF update
        nlnode->cf->N = 0;
        nlnode->cf->LS = VEC::zeros();
        nlnode->cf->SS = 0.;
        for (auto child : nlnode->entries) {
            findCandidates(child, candidates);
            // updating CF after deleting nodes in lower lvls
            nlnode->cf->N += child->cf->N;
            nlnode->cf->LS += child->cf->LS;
            nlnode->cf->SS += child->cf->SS;
        }
    } else {
        std::shared_ptr<LNode> lnode = std::dynamic_pointer_cast<LNode>(node);
        for(auto it = lnode->entries.begin(); it != lnode->entries.end();) {
            if (isCandidate(*it)) {
                candidates.push_back(*it);
//                lnode->entries[i].reset();
                lnode->cf->N -= it->get()->N;
                lnode->cf->LS -= it->get()->LS;
                lnode->cf->SS -= it->get()->SS;
                lnode->entries.erase(it);
                Global::get().getTree()->subcluster_count--;
                continue;
            }
            ++it;
        }
    }
}

bool isCandidate(std::shared_ptr<CF>& cf) { //TODO: find a good way to identify a outlier candidate
    double density = cf->getDensity();
//    double point_count = cf->N;
    if (density == 0) {
//        std::cout << "density = " << density << ", N = " << point_count << std::endl;
        return true;
    } else {
        return false;
    }
}

void tryInsertCandidates(std::vector<std::shared_ptr<CF>>& candidates) {
    outliers_removed_subclusters = 0;
    outliers_removed_points = 0;
    for(auto cand : candidates) {
        bool inserted = insertCandidate(Global::get().getTree()->root, cand);
        if (!inserted) {
            outliers_removed_subclusters++;
            outliers_removed_points += cand->N;
        }
    }
    std::cout << "outliers removed subclusts = " << outliers_removed_subclusters
              << ", points = " << outliers_removed_points
              << ", or = " << candidates.size() << std::endl;
}

bool insertCandidate(std::shared_ptr<BNode> node, std::shared_ptr<CF> candidate) {
    if (!node->isLeafNode()) {
        std::shared_ptr<NLNode> nlnode = std::dynamic_pointer_cast<NLNode>(node);
        std::shared_ptr<BNode> closest_next = findNodesClosestEntry(nlnode, candidate->getCentroid());
        bool inserted = insertCandidate(closest_next, candidate);
        if (inserted) {
            nlnode->cf->N += candidate->N;
            nlnode->cf->LS += candidate->LS;
            nlnode->cf->SS += candidate->SS;
        }
        return inserted;
    } else {
        std::shared_ptr<LNode> lnode = std::dynamic_pointer_cast<LNode>(node);
        std::shared_ptr<CF> closest_subclust = findNodesClosestEntry(lnode, candidate->getCentroid());
        if (closest_subclust->getClusterSize(candidate->getCentroid()) < Global::get().getThreshold_T()) { // can absorbe it?
            closest_subclust->N += candidate->N;
            closest_subclust->LS += candidate->LS;
            closest_subclust->SS += candidate->SS;
            lnode->cf->N += candidate->N;
            lnode->cf->LS += candidate->LS;
            lnode->cf->SS += candidate->SS;
            return true;
        } else {
            return false;
        }
        return false;
    }
}