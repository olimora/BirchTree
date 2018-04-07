//
// Created by omora on 2018-03-29.
//

#include "utils_check_tree.h"
#include "include_global.h"

void checkTheTree(std::shared_ptr<NLNode> node);
void checkTheTree(std::shared_ptr<LNode> node);

void checkTheTree(std::shared_ptr<CFTree> tree) {
    int x = int(tree->root->entries.size());
    printf("Number of entries in root: %d, points: %d\n", x, tree->root->cf->N);
    if (x > 0) {
        for (int i = 0; i < x; i++) {
            printf("N = %d; Cluster size = %f;\n",
                   tree->root->entries[i]->cf->N,
                   tree->root->entries[i]->cf->getClusterSize());
        }
    }
    checkTheTree(tree->root);
}

void checkTheTree(std::shared_ptr<NLNode> node) {
    int x = int(node->entries.size());
    Global::get().lvl++;
    printf("LVL: %d; Entries in node: %d; N: %d; LS0: %f, SS: %f\n", Global::get().lvl, x, node->cf->N, node->cf->LS[0], node->cf->SS);
    for (int i = 0; i < x; i++) {
        std::shared_ptr<BNode> ent = node->entries[i];
        printf("entry node: %d: is Leaf?: %d\n", i, ent->isLeafNode());
        if (ent->isLeafNode() == true) {
            checkTheTree(std::dynamic_pointer_cast<LNode>(ent));
        } else {
            checkTheTree(std::dynamic_pointer_cast<NLNode>(ent));
        }
    }
    Global::get().lvl--;
}

void checkTheTree(std::shared_ptr<LNode> node) {
    int x = int(node->entries.size());
    Global::get().lvl++;
    printf("LVL: %d; Entries in leaf node %p: %d; N: %d; LS0: %f, SS: %f\n", Global::get().lvl, (void *)node.get(), x, node->cf->N, node->cf->LS[0], node->cf->SS);
    for (int i = 0; i < x; i++) {
        printf("Leaf Entry CF: N: %d, LS0: %f, SS %f\n", node->entries[i]->N, node->entries[i]->LS[0], node->entries[i]->SS);
    }
    Global::get().lvl--;////
}