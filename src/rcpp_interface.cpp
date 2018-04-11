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

/**
 *
 * @param data
 * @param BF_B
 * @param BF_L
 * @param threshold_T
 * @param distance_metric
 * @param cluster_size_metric
 * @param memory_limit_MB
 * @param subcluster_limit
 * @param rebuild_size_factor -- 1 default - the bigger, the smaller is new T, and change in tree size
 */
// [[Rcpp::export]]
void buildTree(NumericMatrix data, int BF_B, int BF_L, double threshold_T,
               std::string distance_metric = "euclid", std::string cluster_size_metric = "radius",
               int memory_limit_MB = 0, int subcluster_limit = 50000, double rebuild_size_factor = 1) {
    int rows = data.nrow();
    int cols = data.ncol();

    // set globals
    Global::get().setDimensions(cols);
    Global::get().setBF_B(BF_B);
    Global::get().setBF_L(BF_L);
    Global::get().setThreshold_T(threshold_T);
    Global::get().setDistanceMetric(distance_metric);
    Global::get().setClusterSizeMetric(cluster_size_metric);
    Global::get().setMemoryLimitMB(memory_limit_MB);
    Global::get().setSubclusterLimit(subcluster_limit);
    Global::get().setRebuildSizeFactor(rebuild_size_factor);
    Global::get().rebuild_count = 0;

    std::cout << "Building CF-Tree start" << std::endl;

    // build empty tree
    Global::get().makeTree();

    // add point by point
    Vec point = VEC::zeros();
    for (int r = 0; r < rows; r++) { //
        // make point from data
        for (int c = 0; c < cols; c++) {
            point[c] = data[getIndexInMatrix(r, c, rows)];
        }

        // add point to tree
        addPointToTree(Global::get().getTree(), point);
    }

    std::cout << "Building CF-Tree end" << std::endl;
    std::cout << "Rebuild count = " << Global::get().rebuild_count << std::endl;

//    checkTheTree(tree);

//    std::vector<int> nodesCounts = cauntNodesCounts(tree);
//    std::cout << "Nodes Counts control func: NLNode = " << nodesCounts[0]
//              << "; LNode = " << nodesCounts[1]
//              << "; Subclusters = " << nodesCounts[2]
//              << "; Memory = " << nodesCounts[3] << std::endl;

    std::cout << "Nodes Counts mantained: NLNode = " << Global::get().getTree()->NLNode_count
            << "; LNode = " << Global::get().getTree()->LNode_count
            << "; Subclusters = " <<Global::get().getTree()->subcluster_count << std::endl;

    std::cout << "Total Physical: " << MEMORY::getTotalPhysical() << std::endl;
    std::cout << "Available Physical: " << MEMORY::getAvailablePhysical() << std::endl;
    std::cout << "Consumed Physical: " << MEMORY::getConsumedPhysical() << std::endl;

    std::cout << "Total Virtual: " << MEMORY::getTotalVirtual() << std::endl;
    std::cout << "Available Virtual: " << MEMORY::getAvailableVirtual() << std::endl;
    std::cout << "Consumed Virtual: " << MEMORY::getConsumedVirtual() << std::endl;

    return;
}
