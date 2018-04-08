#include <Rcpp.h>
#include "include_basic.h"
#include "utils_vector.h"
#include "include_global.h"
#include <memory>
#include <string>
#include "algo_build_tree.h"
#include "utils_check_tree.h"
#include "utils_nodes_counts.h"
#include "utils_memory_consumption.h"


using namespace Rcpp;

int getIndexInMatrix(int row, int col, int rows) {
    return col * rows + row;
}

// [[Rcpp::export]]
void buildTree(NumericMatrix data, int BF_B, int BF_L, double threshold_T,
               std::string distance_metric = "euclid", std::string cluster_size_metric = "radius") {
    int rows = data.nrow();
    int cols = data.ncol();

    // set globals
    Global::get().setDimensions(cols);
    Global::get().setBF_B(BF_B);
    Global::get().setBF_L(BF_L);
    Global::get().setThreshold_T(threshold_T);
    Global::get().setDistanceMetric(distance_metric);
    Global::get().setClusterSizeMetric(cluster_size_metric);
    
    std::cout << "Building CF-Tree start" << std::endl;

    // build empty tree
    std::shared_ptr<CFTree> tree = Global::get().makeNewTree();

    // add point by point
    Vec point = vec::zeros();
    for (int r = 0; r < rows; r++) { //
        // make point from data
        for (int c = 0; c < cols; c++) {
            point[c] = data[getIndexInMatrix(r, c, rows)];
        }

        // add point to tree
        addPointToTree(tree, point);
    }

    std::cout << "Building CF-Tree end" << std::endl;

//    checkTheTree(tree);

//    std::vector<int> nodesCounts = cauntNodesCounts(tree);
//    std::cout << "Nodes Counts control func: NLNode = " << nodesCounts[0]
//              << "; LNode = " << nodesCounts[1]
//              << "; Subclusters = " << nodesCounts[2]
//              << "; Memory = " << nodesCounts[3] << std::endl;

    std::cout << "Nodes Counts mantained: NLNode = " << tree->NLNode_count
            << "; LNode = " << tree->LNode_count
            << "; Subclusters = " <<tree->subclusters << std::endl;

    std::cout << "Total Physical: " << memory::getTotalPhysical() << std::endl;
    std::cout << "Available Physical: " << memory::getAvailiblePhysical() << std::endl;
    std::cout << "Consumed Physical: " << memory::getConsumedPhysical() << std::endl;

    std::cout << "Total Virtual: " << memory::getTotalVirtual() << std::endl;
    std::cout << "Available Virtual: " << memory::getAvailibleVirtual() << std::endl;
    std::cout << "Consumed Virtual: " << memory::getConsumedVirtual() << std::endl;

    return;
}
