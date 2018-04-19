//
// Created by omora on 2018-04-13.
//

#include "algo_outliers_removal.h"
#include "algo_rebuild_tree.h"
#include "include_global.h"
#include "algo_utils.h"
#include <map>

int outliers_removed_subclusters;
int outliers_removed_points;
float border_N;

void findCandidates(std::shared_ptr<BNode> node, std::vector<std::shared_ptr<CF>>& candidates);
bool isCandidate(std::shared_ptr<CF>& cf);
void tryInsertCandidates(std::vector<std::shared_ptr<CF>>& candidates);
bool insertCandidate(std::shared_ptr<BNode> node, std::shared_ptr<CF> candidate);
void fillHistogramN(std::shared_ptr<BNode> node, std::map<int, int> &histogram_N);
float findDerivativeChange(std::map<int, int>& histogram_N, float max);

void removeOutliers() {
    std::cout << "/// Outlier Removal ///////////////////////////////////////////" << std::endl;
    // rebuild tree with the same threshold_T, to merge leaf entries, that are not outleirs
    std::cout << "Rebuilding tree to merge leaf entries." << std::endl;
    for (int i = 0; i < Global::get().getRebuildCount(); i++) {
        rebuildTreeBeforeOutliersRemoval();
    }

    std::cout << "Root N count = " << Global::get().getTree()->root->cf->N << std::endl;

    // go through subclusters, collect nodecount, or nodecount * density to vector
    // histogram of that values
    // sort values
    // find 0.01 percentile value = size() .. 100%; x .. 0.01%; x is index in histogram
    // there is target value - subclusters with lower have to be removed / tried to reinsert first
    std::map<int, int> histogram_N;
    fillHistogramN(Global::get().getTree()->root, histogram_N);

    float Min_N = std::pow(Global::get().getTree()->root->cf->N, 0.1);
    float Max_N = std::pow(Global::get().getTree()->root->cf->N, 0.2);
    float Target_N = std::pow(Global::get().getTree()->root->cf->N, 0.15);
    float derivative_change = findDerivativeChange(histogram_N, Max_N);
    float candidate_N = derivative_change;
    if (candidate_N <= Min_N) candidate_N = std::ceil(Min_N);
    if (candidate_N >= Max_N) candidate_N = std::floor(Max_N);
    border_N = candidate_N;

    for (auto it : histogram_N) {
        std::cout << "Histogram N = " << it.first << ", count = " << it.second << std::endl;
    }
    std::cout << "Histogram length = " << histogram_N.size() << std::endl;
    std::cout << "Min_N = " << Min_N << "; Max_N = " << Max_N << "; Target_N = " << Target_N
              << "; candid_N = " << candidate_N << "; border_N = " << border_N << std::endl;

    // go through subclusters and select candidates for subclusters
    std::vector<std::shared_ptr<CF>> candidates;
    findCandidates(Global::get().getTree()->root, candidates);
    std::cout << "Candidates count = " << candidates.size() << std::endl;
    std::cout << "Root N count = " << Global::get().getTree()->root->cf->N << std::endl;

    // try to insert (absorbe) candidates to tree without growing
    tryInsertCandidates(candidates);
    std::cout << "Candidates count after reinsertion = " << candidates.size() << std::endl;
    std::cout << "Root N count = " << Global::get().getTree()->root->cf->N << std::endl;
}

float findDerivativeChange(std::map<int, int>& histogram_N, float max) {
    auto begining = histogram_N.begin();
    int prev_N = (*begining).first;
    int prev_hist = (*begining).second;
    int prev_diff = (*begining).second;
    int act_N, act_hist, act_diff, prev_prev_N = prev_N;
    begining++;

    for (auto it = begining; it != histogram_N.end(); it++)
    {
        if ((*it).first > max) {
            std::cout << "returning ......... max " << max << std::endl;
            return max;
        }
        act_N = (*it).first;
        act_hist = (*it).second;
        act_diff = prev_hist - act_hist;
        std::cout << "prev_hist = " << prev_hist << ", prev_diff = " << prev_diff
                  << ", act_hist = " << act_hist << ", act_diff = " << act_diff << std::endl;
        if (prev_hist <= act_hist) { // hist count derrivative change found
            std::cout << "returning ... hist ... prev " << prev_N << std::endl;
            return prev_N;
        }
        if (prev_diff <= act_diff) { // hist count diff derivative change found
            std::cout << "returning ... diff ... prev " << prev_N << std::endl;
            return prev_prev_N;
        }
        // before next iteration
        prev_prev_N = prev_N;
        prev_N = act_N;
        prev_hist = act_hist;
        prev_diff = act_diff;
    }
    return max;
}

void fillHistogramN(std::shared_ptr<BNode> node, std::map<int, int> &histogram_N) {
    if (!node->isLeafNode()) {
        std::shared_ptr<NLNode> nlnode = std::dynamic_pointer_cast<NLNode>(node);
        for (auto child : nlnode->entries) {
            fillHistogramN(child, histogram_N);
        }
    } else {
        std::shared_ptr<LNode> lnode = std::dynamic_pointer_cast<LNode>(node);
        for (auto child : lnode->entries) {
            // calculate the score
            float act_score = child->N;
            // try find it in validityScores vector
            auto it = histogram_N.find(act_score);
            // if not there, add it
            if (it == histogram_N.end()) {
                histogram_N[act_score] = 1;
            } else {
                it->second++;
            }
        }
    }
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

bool isCandidate(std::shared_ptr<CF>& cf) {
    if (cf->N <= border_N) {
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
            Global::get().outliers.push_back(cand);
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