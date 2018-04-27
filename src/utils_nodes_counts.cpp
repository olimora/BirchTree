//
// Created by omora on 2018-04-06.
//

#include "utils_nodes_counts.h"

void countNodes(std::shared_ptr<BNode> node, std::vector<int> &counts) {
    if (node->isLeafNode()) {
        counts[1]++;

        // subclusters
        auto lnode = std::dynamic_pointer_cast<LNode>(node);
        counts[2] += lnode->entries.size();

    } else {
        counts[0]++;

        // and recursively to children
        auto nlnode = std::dynamic_pointer_cast<NLNode>(node);
        for (std::vector<int>::size_type i = 0; i < nlnode->entries.size(); i++) {
            countNodes(nlnode->entries[i], counts);
        }
    }
}

std::vector<int> cauntNodesCounts(std::shared_ptr<CFTree> tree) {
    std::vector<int> counts(3); //[0] NLNodes, [1] LNodes, [2] subclusters

    //call recursive function to tree and accumulate the counts
    countNodes(tree->root, counts); // starts with root

    return counts;
}

