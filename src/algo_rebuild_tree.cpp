//
// Created by omora on 2018-04-08.
//

#include "algo_rebuild_tree.h"
#include "utils_memory_consumption.h"
#include "include_global.h"
#include "iostream"
#include "algo_rebuild_utils.h"
//#include "utils_check_tree.h" //TODO: vymazat

const float MEMORY_RESERVE_MB = 128.;

void calculateNewThresholdT();
void recursiveThresholdCall(std::shared_ptr<BNode> node);
void rebuildTree();

void rebuildTreeIfNeeded() {

    // check if is enough memory
    bool physical_memory_limit_exceeded = false;
    bool allowed_memory_limit_exceeded = false;

    if (MEMORY::getAvailablePhysical() <= MEMORY_RESERVE_MB) {
        physical_memory_limit_exceeded = true;
        std::cout << "Available memory limit exceeded. ";
    }
    if (Global::get().getMemoryLimitMB() > 0
        && MEMORY::getConsumedPhysical() >= Global::get().getMemoryLimitMB()) {
        allowed_memory_limit_exceeded = true;
        std::cout << "Allowed memory limit exceeded. ";
    }

    // check subcluster count
    bool subcluster_count_limit_exceeded = false;
    if (Global::get().getTree()->subcluster_count > Global::get().getSubclusterLimit()) {
        subcluster_count_limit_exceeded = true;
        std::cout << "Subcluster count limit exceeded. ";
    }

    if (physical_memory_limit_exceeded || allowed_memory_limit_exceeded || subcluster_count_limit_exceeded) {
        std::cout << "Need to rebuild the tree." << std::endl;

        Global::get().setMemoryMax(MEMORY::getConsumedPhysical());
        std::cout << "Available phys = " << MEMORY::getAvailablePhysical()
                  << "; Consumed phys = " << MEMORY::getConsumedPhysical()
                  << "; Subclusters = " << Global::get().getTree()->subcluster_count << std::endl;


        // calculate new Threshold_T
        calculateNewThresholdT();

        std::cout << "Rebuilding tree. " << std::endl;
        rebuildTree();
        Global::get().rebuild_count++;
        std::cout << "/// Rebuilding Tree ///////////////////////////////" << std::endl;
//        throw std::invalid_argument( "Rebuilding tree. ");
    }
}

void rebuildTreeBeforeOutliersRemoval() {
    rebuildTree();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int STARTING_LVL = 0;

void recursiveRebuild(std::shared_ptr<BNode> node);
void copyNode(std::shared_ptr<BNode> original, std::shared_ptr<BNode> actual, int lvl);
bool insertLeafEntryInClosestPath(std::shared_ptr<CF> original, std::shared_ptr<BNode> actual);
void insertLeafEntryInCurrentPath(std::shared_ptr<CF> original, std::shared_ptr<BNode> actual, int lvl);
void updateNewTree(std::shared_ptr<BNode>& node);

void rebuildTree() {

    // reset Rebuilder
    Rebuilder::get().resetPaths();
    Rebuilder::get().makeNewTree();
    Rebuilder::get().getTree()->root->cf->N = Global::get().getTree()->root->cf->N;
    Rebuilder::get().getTree()->root->cf->LS = Global::get().getTree()->root->cf->LS;
    Rebuilder::get().getTree()->root->cf->SS = Global::get().getTree()->root->cf->SS;

    std::cout << "Nodes Counts mantained Before: NLNode = " << Global::get().getTree()->NLNode_count
              << "; LNode = " << Global::get().getTree()->LNode_count
              << "; Subclusters = " <<Global::get().getTree()->subcluster_count << std::endl;

    recursiveRebuild(Global::get().getTree()->root);

    std::shared_ptr<BNode> root = std::dynamic_pointer_cast<BNode>(Rebuilder::get().getTree()->root);
    updateNewTree(root);

    std::cout << "Nodes Counts mantained After: NLNode = " << Rebuilder::get().getTree()->NLNode_count
              << "; LNode = " << Rebuilder::get().getTree()->LNode_count
              << "; Subclusters = " << Rebuilder::get().getTree()->subcluster_count << std::endl;

//    std::cout << "Available phys = " << MEMORY::getAvailablePhysical()
//              << "; Consumed phys = " << MEMORY::getConsumedPhysical()
//              << "; Available virt = " << MEMORY::getAvailableVirtual()
//              << "; Consumed virt = " << MEMORY::getConsumedVirtual() << std::endl;

    // destroy old tree and put there the new one
    Global::get().getTree().reset();
    Global::get().setTree(Rebuilder::get().getTree());
}

void recursiveRebuild(std::shared_ptr<BNode> node) {
    // copy this node to new tree if it is not there == create new LNode in right place in new tree
    copyNode(node, Rebuilder::get().getTree()->root, STARTING_LVL);

    if (!node->isLeafNode()) { // not leaf node
        // identify node by its Path (old current)
        auto nlnode = std::dynamic_pointer_cast<NLNode>(node);
        for (int i = 0; i < int(nlnode->entries.size()) ; i++) {
            Rebuilder::get().old_current_path.push_back(i);
            recursiveRebuild(nlnode->entries[i]);
            Rebuilder::get().old_current_path.pop_back();
            nlnode->entries[i].reset();
        }
        nlnode->entries.clear();
    } else {            // leaf node
        // leaf node - try to insert entries to the new tree
        auto lnode = std::dynamic_pointer_cast<LNode>(node);
        for (int i = 0; i < int(lnode->entries.size()) ; i++) {
            Rebuilder::get().new_closest_path.clear(); //reset NCP
            bool inserted = insertLeafEntryInClosestPath(lnode->entries[i], Rebuilder::get().getTree()->root);
            if (!inserted) {
//                std::cout << "Not inserted in closest path, inserting in OCP" << std::endl;
                insertLeafEntryInCurrentPath(lnode->entries[i], Rebuilder::get().getTree()->root, STARTING_LVL);
            } else {
//                std::cout << "Inserted in closest path" << std::endl;
            }
            lnode->entries[i].reset();
        }
        lnode->entries.clear();
    }
    node.reset();
}

void copyNode(std::shared_ptr<BNode> original, std::shared_ptr<BNode> actual, int lvl) {
    if (Rebuilder::get().old_current_path.size() == 0) { // got called from root - root is already created, do nothing
        return;
    }
    // create actual->entries[indx] node as original node is
    // indx = old_current_path[lvl]
    if (lvl+1 == int(Rebuilder::get().old_current_path.size())) { // now add new node
        int indx = Rebuilder::get().old_current_path[lvl];
        std::shared_ptr<NLNode> actuall = std::dynamic_pointer_cast<NLNode>(actual);

        if (indx == int(actuall->entries.size())) { // good, push back new node to entries
            std::shared_ptr<BNode> new_node;
            if (original->isLeafNode()) {
                new_node = std::make_shared<LNode>();
                Rebuilder::get().getTree()->LNode_count++;
            } else {
                new_node = std::make_shared<NLNode>();
                Rebuilder::get().getTree()->NLNode_count++;
            }
            new_node->cf->N = original->cf->N;
            new_node->cf->LS = original->cf->LS;
            new_node->cf->SS = original->cf->SS;
            actuall->entries.push_back(new_node);
            return;
        } else if (indx+1 == int(actuall->entries.size())) { // this node is already created
            // check if it is the same node  // just for control
            if (original->cf->N == actuall->entries[indx]->cf->N
                && original->cf->SS == actuall->entries[indx]->cf->SS) {
                // it is the same do nothing;
                return;
            } else {
                throw std::logic_error( "Badly constructed new tree. Ex 0. ");
            }
        } else {
            throw std::logic_error( "Badly constructed new tree. Ex 1. ");
        }
        return;
    }
    if (lvl+1 < int(Rebuilder::get().old_current_path.size())) { // traverse down the path
        int indx = Rebuilder::get().old_current_path[lvl];
        auto nlactual = std::dynamic_pointer_cast<NLNode>(actual);
        if (indx >= int(nlactual->entries.size())) { // throw error, not properly build, it should have been here
            throw std::logic_error( "Badly constructed new tree. Ex 2. ");
        }
        copyNode(original, nlactual->entries[indx], lvl + 1);
        return;
    }
    //else lvl+1 > Rebuilder::get().old_current_path.size() - throw error
    throw std::logic_error( "Badly constructed new tree. Ex 3. ");

}

bool insertLeafEntryInClosestPath(std::shared_ptr<CF> original, std::shared_ptr<BNode> actual) {
    // manage new_closest_path with traversing
    // traverse the tree to closest node on every lvl, add index to the NCP
    // upon reaching leaf node, try to insert : find closest entry:
    // if has no entries :  push back new CF copied from original to entries
    // if has entries : can closest absorb it with new threshold? -> merge them (new CF values '+')
    //                                                        no -> has it room for another entry? -> push back new CF copied from original to entries
    //                                                                                          no -> return false; insert entry in old_current_path in new tree

    // this starts at root, with empty new_closest_path
    if (!actual->isLeafNode()) { // traverse down the path
        // not in leaf yet, so actual is NLNode
        std::shared_ptr<NLNode> actuall = std::dynamic_pointer_cast<NLNode>(actual);

        // find closest entry, add its index to NCP, and recursive call to that node
        int closest = findNodesClosestEntry(actuall->entries, original->getCentroid());
        Rebuilder::get().new_closest_path.push_back(closest);
        return insertLeafEntryInClosestPath(original, actuall->entries[closest]);
    } else {
        // in leaf node, so actual is NLNode, insert here
        std::shared_ptr<LNode> actuall = std::dynamic_pointer_cast<LNode>(actual);
        if (actuall->entries.size() == 0) { // if has no entries :  push back new CF copied from original to entries, return true
            actuall->entries.push_back(std::make_shared<CF>(original->N, original->LS, original->SS));
            Rebuilder::get().getTree()->subcluster_count++;
            return true;
        } else {
            // find closest entry, can it absorbe inserting entry?
            std::shared_ptr<CF> closest_ent = findNodesClosestEntry(actuall->entries, original->getCentroid());
            if (closest_ent->getClusterSize(original->getCentroid()) < Global::get().getThreshold_T()) { // can absorbe it, merge CFs
                closest_ent->N += original->N;
                closest_ent->LS += original->LS;
                closest_ent->SS += original->SS;
                return true;
            } else { // cant absorbe it, has it room for another entry? then add it
                if (int(actuall->entries.size()) < Global::get().getBF_L()) { // can fit in without splitting
                    actuall->entries.push_back(std::make_shared<CF>(original->N, original->LS, original->SS));
                    Rebuilder::get().getTree()->subcluster_count++;
                    return true;
                } else {
                    return false; // need to insert it in old_current_path
                }
            }
        }
    }
}

void insertLeafEntryInCurrentPath(std::shared_ptr<CF> original, std::shared_ptr<BNode> actual, int lvl) {
    if (!actual->isLeafNode()) { // not leaf yet, traverse down by the path
        std::shared_ptr<NLNode> actuall = std::dynamic_pointer_cast<NLNode>(actual);
        int indx = Rebuilder::get().old_current_path[lvl];
        insertLeafEntryInCurrentPath(original, actuall->entries[indx], lvl+1);
    } else {  // in leaf node, just add it here, can control the size of entries vector just in case
        std::shared_ptr<LNode> actuall = std::dynamic_pointer_cast<LNode>(actual);
        if (int(actuall->entries.size()) < Global::get().getBF_L()) {
            actuall->entries.push_back(std::make_shared<CF>(original->N, original->LS, original->SS));
            Rebuilder::get().getTree()->subcluster_count++;
        } else {
            throw std::logic_error( "Badly constructed new tree. Ex 4. ");
        }
    }
}

void updateNewTree(std::shared_ptr<BNode>& node) {
    // first, recursive call to all children == entries, to update them
    // then, when I got updated entries, I can update my own CF
    if (!node->isLeafNode()) { // not leaf
        std::shared_ptr<NLNode> nlnode = std::dynamic_pointer_cast<NLNode>(node);
        if (nlnode->entries.size() == 0) { // if has no entries, reset == delete this from parent as well
            nlnode.reset();
            node.reset();
            return;
        }
        // reset this CF
        nlnode->cf->N = 0;
        nlnode->cf->LS = VEC::zeros();
        nlnode->cf->SS = 0.;
        // update all entries, and updated entry can be added to CF
        for(auto it = nlnode->entries.begin(); it != nlnode->entries.end();) {
            updateNewTree(*it);
            if (*it == nullptr) { // entry was reset()
                it = nlnode->entries.erase(it);
                continue;
            }
            nlnode->cf->N += it->get()->cf->N;
            nlnode->cf->LS += it->get()->cf->LS;
            nlnode->cf->SS += it->get()->cf->SS;
            ++it;
        }
        if (nlnode->entries.size() == 0) { // if has no entries, reset == delete this from parent as well
            nlnode.reset();
            node.reset();
            Rebuilder::get().getTree()->NLNode_count--;
            return;
        }
    } else { // leaf node
        std::shared_ptr<LNode> lnode = std::dynamic_pointer_cast<LNode>(node);
        if (lnode->entries.size() == 0) { // if has no entries, reset == delete this from parent as well
            lnode.reset();
            node.reset();
            return;
        }
        // reset this CF
        lnode->cf->N = 0;
        lnode->cf->LS = VEC::zeros();
        lnode->cf->SS = 0.;
        // update all entries, and updated entry can be added to CF
        for (unsigned int i = 0; i < lnode->entries.size(); i++) {
            lnode->cf->N += lnode->entries[i]->N;
            lnode->cf->LS += lnode->entries[i]->LS;
            lnode->cf->SS += lnode->entries[i]->SS;
        }
    }
}