#include "algo_build_tree.h"
#include "algo_utils.h"
#include "include_global.h"
#include "algo_rebuild_tree.h"

void addPointToTree(std::shared_ptr<CFTree> tree, Vec point);

std::vector<std::shared_ptr<NLNode>> addPointToNode(std::shared_ptr<NLNode> node, Vec point);
std::vector<std::shared_ptr<LNode>> addPointToLeafNode(std::shared_ptr<LNode> node, Vec point);

void mergeNodes(std::shared_ptr<NLNode> node1, std::shared_ptr<NLNode> node2, std::shared_ptr<NLNode> parent);
void mergeNodes(std::shared_ptr<LNode> node1, std::shared_ptr<LNode> node2, std::shared_ptr<NLNode> parent);

std::vector<std::shared_ptr<NLNode>> splitNode(std::shared_ptr<NLNode> node, std::shared_ptr<BNode> node_plus);
std::vector<std::shared_ptr<LNode>> splitNode(std::shared_ptr<LNode> node, Vec point);

std::shared_ptr<LNode> makeNewLeafNodeWithPoint(Vec point);

// first (parent) function in insertion algorithm
void addPointToTree(std::shared_ptr<CFTree> tree, Vec point) {
    // call 'add point to node' function on root. returns vector of nodes:
    //   - if one node, no splitting in lover lvl, just update CF
    //   - if two nodes, need to split root node = make new root node and add those two as his entries
    std::vector<std::shared_ptr<NLNode>> returned = addPointToNode(tree->root, point);
    if (returned.size() == 1) {  // if one node. no splitting in lover lvl, just update CF //TODO:: is cf updated in lower lvl?
//        tree->point_count++;
    } else if (returned.size() == 2) { // if two nodes, need to split root node = make new root node and add those two as his entries
        tree->root = std::make_shared<NLNode>();
        tree->root->entries.push_back(returned[0]);
        tree->root->entries.push_back(returned[1]);
        tree->root->cf = std::make_shared<CF>(returned[0]->cf, returned[1]->cf);
//        tree->point_count++;
        tree->NLNode_count++;
    } else { // cant happen
        throw std::logic_error( "Returned invalid number of nodes to root in insertion process." );
    }

    // check for rebuilding algorithm
    rebuildTreeIfNeeded();
}

std::vector<std::shared_ptr<NLNode>> addPointToNode(std::shared_ptr<NLNode> node, Vec point) {
    std::vector<std::shared_ptr<NLNode>> returning;
    returning.reserve(2);

    // recursivly traverse down to nearest entry
    if (node->entries.size() > 0) { //has entries. find closes one and recursivly traverse down to nearest entry
        std::shared_ptr<BNode> closest_ent = findNodesClosestEntry(node, point);
        if (closest_ent->isLeafNode() == false) { // Non Leaf NLNode - recursive to nearest entry
            std::vector<std::shared_ptr<NLNode>> returned = addPointToNode(std::dynamic_pointer_cast<NLNode>(closest_ent), point);
            if (returned.size() == 1) {             // no splitting happened
                node->cf->addPoint(point);
                returning.push_back(node);
                return returning;
            } else if  (returned.size() == 2) {     // splitting happened
                if (int(node->entries.size()) < Global::get().getBF_B()) { // this node can hold one more entry (leaf node)
                    //  returned node split in two, and now it can be absorbed, no need to splitting higher in the tree
                    node->entries.push_back(returned[1]);

                    //merging refinement:
                    // 1. find 2 closest entries
                    std::vector<std::shared_ptr<BNode>> closest_ents = findClosestEntriesPair(node->entries);
                    // 2. if these are the split result. do nothing. so check otherwise and only then do merge
                    if (!((std::dynamic_pointer_cast<NLNode>(closest_ents[0]) == returned[0] &&
                                std::dynamic_pointer_cast<NLNode>(closest_ents[1]) == returned[1]) ||
                          (std::dynamic_pointer_cast<NLNode>(closest_ents[1]) == returned[0] &&
                                std::dynamic_pointer_cast<NLNode>(closest_ents[0]) == returned[1])))  {
                        // now try to merge them
                        mergeNodes(std::dynamic_pointer_cast<NLNode>(closest_ents[0]),
                                std::dynamic_pointer_cast<NLNode>(closest_ents[1]), node);
                    }

                    node->cf->addPoint(point);
                    returning.push_back(node); //return only this one - we did not split this node
                    return returning;
                } else {      // need to split this node
                    return splitNode(node, returned[1]);
                }
            } else {
                throw std::logic_error( "Returned invalid number of nodes from addPointToNode. maybe after splitting entry node." );
            }
        } else { // Leaf NLNode - call add to Leaf NLNode func
            std::vector<std::shared_ptr<LNode>> returned = addPointToLeafNode(std::dynamic_pointer_cast<LNode>(closest_ent), point);
            if (returned.size() == 1) { // no splitting happened
                node->cf->addPoint(point);
                returning.push_back(node);
                return returning;
            } else if  (returned.size() == 2) {      // splitting happened
                if (int(node->entries.size()) < Global::get().getBF_B()) { // this node can hold one more entry (leaf node)
                    //  came node split in two, and now it can be absorbed, no need to splitting higher in the tree
                    node->entries.push_back(returned[1]);

                    //merging refinement:
                    // 1. find 2 closest entries
                    std::vector<std::shared_ptr<BNode>> closest_ents = findClosestEntriesPair(node->entries);
                    // 2. if these are the split result. do nothing. so check otherwise and only then do merge
                    if (!((std::dynamic_pointer_cast<LNode>(closest_ents[0]) == returned[0] &&
                            std::dynamic_pointer_cast<LNode>(closest_ents[1]) == returned[1]) ||
                          (std::dynamic_pointer_cast<LNode>(closest_ents[1]) == returned[0] &&
                            std::dynamic_pointer_cast<LNode>(closest_ents[0]) == returned[1])))  {
                        // now try to merge them
                        mergeNodes(std::dynamic_pointer_cast<LNode>(closest_ents[0]), std::dynamic_pointer_cast<LNode>(closest_ents[1]), node);
                    }

                    node->cf->addPoint(point);
                    returning.push_back(node); //return only this one - we did not split this node
                    return returning;
                } else {      // need to split this node
                    return splitNode(node, returned[1]);
                }
            } else {
                throw std::logic_error( "Returned invalid number of nodes from addPointToLeafNode. maybe after splitting leaf node." );
            }
        }
    } else { // does not have any entry. need to add LeafNode, and place point in new LeafNode
        node->entries.push_back(std::make_shared<LNode>(point));
        Global::get().getTree()->subcluster_count++;
        Global::get().getTree()->LNode_count++;
        returning.push_back(node);
        return returning;
    }
}

std::vector<std::shared_ptr<LNode>> addPointToLeafNode(std::shared_ptr<LNode> node, Vec point) {
    std::vector<std::shared_ptr<LNode>> returning;
    returning.reserve(2);

    if (node->entries.size() > 0) { // has entries: find the closest from entries
        std::shared_ptr<CF> closest_ent = findNodesClosestEntry(node, point);
        // if it can absorbe point under T, absorbe it
        if (closest_ent->getClusterSize(point) < Global::get().getThreshold_T()) {
            closest_ent->addPoint(point); // point to cf as entry
            node->cf->addPoint(point);  // point to cf of node
            returning.push_back(node);
            return returning;

        } else { // else - closest entry cant absorb
            if (int(node->entries.size()) < Global::get().getBF_L()) { // add entry if it can hold one more
                node->entries.push_back(std::make_shared<CF>(1, point, VEC::squaredSum(point)));
                Global::get().getTree()->subcluster_count++;
                node->cf->addPoint(point);
                returning.push_back(node);
                return returning;
            } else { // splitting the Leaf Node
                return splitNode(node, point); // returns two nodes after split. it adds new entry (input param point)
            }
        }
    } else { // has no entries, cant search for closest. add one. and is resolved
        node->entries.push_back(std::make_shared<CF>(1, point, VEC::squaredSum(point)));
        Global::get().getTree()->subcluster_count++;
        node->cf->addPoint(point);
        returning.push_back(node);
        return returning;
    }
}

void mergeNodes(std::shared_ptr<NLNode> node1, std::shared_ptr<NLNode> node2, std::shared_ptr<NLNode> parent) {
    // can be nerged? num of entries less than BF_L
    if (int(node1->entries.size() + node2->entries.size()) <= parent->getBF()) { //then make one vector out of it and one merged node. free the other one
        node1->entries.insert(node1->entries.end(), node2->entries.begin(), node2->entries.end());
        node2->entries.clear();
        // dont forget add cf
        node1->cf->N += node2->cf->N;
        node1->cf->LS += node2->cf->LS;
        node1->cf->SS += node2->cf->SS;
        // delete node2
        parent->entries.erase(std::find(parent->entries.begin(), parent->entries.end(), node2));
        node2.reset(); //TODO: delete shared pointer right way
        Global::get().getTree()->NLNode_count--;
    } else { // else resplit
        // make entries one vector
        std::vector<std::shared_ptr<BNode>> all_ents (node1->entries);
        all_ents.insert(all_ents.end(), node2->entries.begin(), node2->entries.end());
        // chose two seeds
        std::vector<std::shared_ptr<BNode>> seeds = findFurthestEntries(all_ents);  // find the fursthest entries = seeds
        // clear old vectors and push seeds to them
        node1->entries.clear();
        node2->entries.clear();
        node1->entries.push_back(seeds[0]);
        node2->entries.push_back(seeds[1]);
        node1->cf->N = seeds[0]->cf->N;             // CFs - reset old and make new
        node1->cf->LS = seeds[0]->cf->LS;
        node1->cf->SS = seeds[0]->cf->SS;
        node2->cf->N = seeds[1]->cf->N;             // CFs - reset old and make new
        node2->cf->LS = seeds[1]->cf->LS;
        node2->cf->SS = seeds[1]->cf->SS;
        // loop all ents and push to seeds update CF. if one vector is full, then push the rest and end it
        double dist1, dist2;
        for (int i = 0; i < int(all_ents.size()); i++) {
            if (all_ents[i] == seeds[0] || all_ents[i] == seeds[1]) {
                continue;
            } else {
                dist1 = distance(all_ents[i]->getCentroid(), node1->entries[0]->getCentroid());
                dist2 = distance(all_ents[i]->getCentroid(), node2->entries[0]->getCentroid());
                if (dist1 <= dist2) {
                    if (int(node1->entries.size()) < node1->getBF()) {
                        node1->entries.push_back(all_ents[i]);
                        node1->cf->N += all_ents[i]->cf->N;
                        node1->cf->LS += all_ents[i]->cf->LS;
                        node1->cf->SS += all_ents[i]->cf->SS;
                    } else {
                        node2->entries.push_back(all_ents[i]);
                        node2->cf->N += all_ents[i]->cf->N;
                        node2->cf->LS += all_ents[i]->cf->LS;
                        node2->cf->SS += all_ents[i]->cf->SS;
                    }
                } else {
                    if (int(node2->entries.size()) < node1->getBF()) {
                        node2->entries.push_back(all_ents[i]);
                        node2->cf->N += all_ents[i]->cf->N;
                        node2->cf->LS += all_ents[i]->cf->LS;
                        node2->cf->SS += all_ents[i]->cf->SS;
                    } else {
                        node1->entries.push_back(all_ents[i]);
                        node1->cf->N += all_ents[i]->cf->N;
                        node1->cf->LS += all_ents[i]->cf->LS;
                        node1->cf->SS += all_ents[i]->cf->SS;
                    }
                }
            }
        }
    }
}

void mergeNodes(std::shared_ptr<LNode> node1, std::shared_ptr<LNode> node2, std::shared_ptr<NLNode> parent) {
    // can be nerged? num of entries less than BF_L
    if (int(node1->entries.size() + node2->entries.size()) <= parent->getBF()) { //then make one vector out of it and one merged node. free the other one
        node1->entries.insert(node1->entries.end(), node2->entries.begin(), node2->entries.end());
        node2->entries.clear();
        // dont forget add cf
        node1->cf->N += node2->cf->N;
        node1->cf->LS += node2->cf->LS;
        node1->cf->SS += node2->cf->SS;
        // delete node2
        parent->entries.erase(std::find(parent->entries.begin(), parent->entries.end(), node2));
        node2.reset(); //TODO: delete shared pointer right way ; print reference count for debugging purposes
        Global::get().getTree()->LNode_count--;
    } else { // else resplit
        // make entries one vector
        std::vector<std::shared_ptr<CF>> all_ents (node1->entries);
        all_ents.insert(all_ents.end(), node2->entries.begin(), node2->entries.end());
        // chose two seeds
        std::vector<std::shared_ptr<CF>> seeds = findFurthestEntries(all_ents);  // find the fursthest entries = seeds
        // clear old vectors and push seeds to them
        node1->entries.clear();
        node2->entries.clear();
        node1->entries.push_back(seeds[0]);
        node2->entries.push_back(seeds[1]);
        node1->cf->N = seeds[0]->N;             // CFs - reset old and make new
        node1->cf->LS = seeds[0]->LS;
        node1->cf->SS = seeds[0]->SS;
        node2->cf->N = seeds[1]->N;             // CFs - reset old and make new
        node2->cf->LS = seeds[1]->LS;
        node2->cf->SS = seeds[1]->SS;
        // loop all ents and push to seeds update CF. if one vector is full, then push the rest and end it
        double dist1, dist2;
        for (int i = 0; i < int(all_ents.size()); i++) {
            if (all_ents[i] == seeds[0] || all_ents[i] == seeds[1]) {
                continue;
            } else {
                dist1 = distance(all_ents[i]->getCentroid(), node1->entries[0]->getCentroid());
                dist2 = distance(all_ents[i]->getCentroid(), node2->entries[0]->getCentroid());
                if (dist1 <= dist2) {
                    if (int(node1->entries.size()) < node1->getBF()) {
                        node1->entries.push_back(all_ents[i]);
                        node1->cf->N += all_ents[i]->N;
                        node1->cf->LS += all_ents[i]->LS;
                        node1->cf->SS += all_ents[i]->SS;
                    } else {
                        node2->entries.push_back(all_ents[i]);
                        node2->cf->N += all_ents[i]->N;
                        node2->cf->LS += all_ents[i]->LS;
                        node2->cf->SS += all_ents[i]->SS;
                    }
                } else {
                    if (int(node2->entries.size()) < node1->getBF()) {
                        node2->entries.push_back(all_ents[i]);
                        node2->cf->N += all_ents[i]->N;
                        node2->cf->LS += all_ents[i]->LS;
                        node2->cf->SS += all_ents[i]->SS;
                    } else {
                        node1->entries.push_back(all_ents[i]);
                        node1->cf->N += all_ents[i]->N;
                        node1->cf->LS += all_ents[i]->LS;
                        node1->cf->SS += all_ents[i]->SS;
                    }
                }
            }
        }
    }
}

std::vector<std::shared_ptr<NLNode>> splitNode(std::shared_ptr<NLNode> node, std::shared_ptr<BNode> node_plus) {
    std::vector<std::shared_ptr<NLNode>> returning;
    returning.reserve(2);

    std::vector<std::shared_ptr<BNode>> old_ents (node->entries);  // copy old vector of entries //if memory problems. copy / swap?
    old_ents.push_back(node_plus);                // add new entry, so it can be chosen as seed too
    std::vector<std::shared_ptr<BNode>> seeds = findFurthestEntries(old_ents);  // find the fursthest entries = seeds
    node->entries.clear();        // clear the original one so we can populate it
    std::shared_ptr<NLNode> new_node = std::make_shared<NLNode>();    // make new node with entries vector to populate
    Global::get().getTree()->NLNode_count++;
    node->entries.push_back(seeds[0]);     // put the seeds first
    new_node->entries.push_back(seeds[1]);
    node->cf->N = seeds[0]->cf->N;             // CFs - reset old and make new
    node->cf->LS = seeds[0]->cf->LS;
    node->cf->SS = seeds[0]->cf->SS;
    new_node->cf = std::make_shared<CF>(seeds[1]->cf->N, seeds[1]->cf->LS, seeds[1]->cf->SS);

    // distribute the rest of entries into vectors/nodes by proximity to seeds
    double dist1, dist2;
    for (int i = 0; i < int(old_ents.size()); i++) {
        if (old_ents[i] == seeds[0] || old_ents[i] == seeds[1]) {
            continue;
        } else { // distance to seeds
            dist1 = distance(old_ents[i]->getCentroid(), node->entries[0]->getCentroid());
            dist2 = distance(old_ents[i]->getCentroid(), new_node->entries[0]->getCentroid());
            if (dist1 <= dist2) {
                node->entries.push_back(old_ents[i]);
                node->cf->N += old_ents[i]->cf->N;
                node->cf->LS += old_ents[i]->cf->LS;
                node->cf->SS += old_ents[i]->cf->SS;
            } else {
                new_node->entries.push_back(old_ents[i]);
                new_node->cf->N += old_ents[i]->cf->N;
                new_node->cf->LS += old_ents[i]->cf->LS;
                new_node->cf->SS += old_ents[i]->cf->SS;
            }
        }
    }
    returning.push_back(node);
    returning.push_back(new_node);
    return returning;
}


std::vector<std::shared_ptr<LNode>> splitNode(std::shared_ptr<LNode> node, Vec point) {
    std::vector<std::shared_ptr<LNode>> returning;
    returning.reserve(2);

    std::vector<std::shared_ptr<CF>> old_ents (node->entries);  // copy old vector of entries //if memory problems. copy / swap?
    old_ents.push_back(std::make_shared<CF>(1, point, VEC::squaredSum(point)));   // add new entry, so it can be chosen as seed too
    Global::get().getTree()->subcluster_count++;
    std::vector<std::shared_ptr<CF>> seeds = findFurthestEntries(old_ents);  // find the fursthest entries = seeds
    node->entries.clear();        // clear the original one so we can populate it
    std::shared_ptr<LNode> new_node = std::make_shared<LNode>();    // make new node with entries vector to populate
    Global::get().getTree()->LNode_count++;
    node->entries.push_back(seeds[0]);     // put the seeds first
    new_node->entries.push_back(seeds[1]);
    node->cf->N = seeds[0]->N;             // CFs - reset old and make new
    node->cf->LS = seeds[0]->LS;
    node->cf->SS = seeds[0]->SS;
    new_node->cf = std::make_shared<CF>(seeds[1]->N, seeds[1]->LS, seeds[1]->SS);

    // distribute the rest of entries into vectors/nodes by proximity to seeds
    double dist1, dist2;
    for (int i = 0; i < int(old_ents.size()); i++) {
        if (old_ents[i] == seeds[0] || old_ents[i] == seeds[1]) {
            continue;
        } else { // distance to seeds
            dist1 = distance(old_ents[i]->getCentroid(), node->entries[0]->getCentroid());
            dist2 = distance(old_ents[i]->getCentroid(), new_node->entries[0]->getCentroid());
            if (dist1 <= dist2) {
                node->entries.push_back(old_ents[i]);
                node->cf->N += old_ents[i]->N;
                node->cf->LS += old_ents[i]->LS;
                node->cf->SS += old_ents[i]->SS;
            } else {
                new_node->entries.push_back(old_ents[i]);
                new_node->cf->N += old_ents[i]->N;
                new_node->cf->LS += old_ents[i]->LS;
                new_node->cf->SS += old_ents[i]->SS;
            }
        }
    }
    returning.push_back(node);
    returning.push_back(new_node);
    return returning;
}