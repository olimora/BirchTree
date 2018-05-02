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
#include "utils_return_matrix.h"
#include "algo_outliers_removal.h"


using namespace Rcpp;

//int getIndexInMatrix(int row, int col, int rows) {
//    return col * rows + row;
//}

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
List buildTree(NumericMatrix data, int BF_B, int BF_L, double threshold_T = 0,
               std::string distance_metric = "euclid", std::string cluster_size_metric = "radius",
               int memory_limit_MB = 0, int subcluster_limit = 50000, double rebuild_size_factor = 1,
               bool remove_outliers = true, int rebuild_count_before_outlier_removal = 2) {

    int rows = data.nrow();
    int cols = data.ncol();

    // set globals
    Global::get().setMemoryWithoutTree(MEMORY::getConsumedPhysical());
    Global::get().setMemoryMax(MEMORY::getConsumedPhysical());
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
    Global::get().setRemoveOutliers(remove_outliers);
    Global::get().setRebuildCount(rebuild_count_before_outlier_removal);

    std::cout << "Building CF-Tree start" << std::endl;

    // build empty tree
    Global::get().makeTree();
    // add point by point
    Vec point = VEC::zeros(); // make point from data
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {   // fill the point values
            point[c] = data[getIndexInMatrix(r, c, rows)];
        }
        addPointToTree(Global::get().getTree(), point); // add point to tree
    }

    std::cout << "Building CF-Tree end" << std::endl;
    std::cout << "Rebuild count = " << Global::get().rebuild_count << std::endl;

    // remove outliers
    if (Global::get().getRemoveOutliers()) {
        removeOutliers();
    }

//    checkTheTree(tree);

//    std::vector<int> nodesCounts = cauntNodesCounts(Global::get().getTree());
//    std::cout << "Nodes Counts control func: NLNode = " << nodesCounts[0]
//              << "; LNode = " << nodesCounts[1]
//              << "; Subclusters = " << nodesCounts[2]
//              << "; Memory = " << nodesCounts[3] << std::endl;

    std::cout << "Nodes Counts mantained: NLNode = " << Global::get().getTree()->NLNode_count
            << "; LNode = " << Global::get().getTree()->LNode_count
            << "; Subclusters = " <<Global::get().getTree()->subcluster_count << std::endl;

//    std::cout << "Total Physical: " << MEMORY::getTotalPhysical() << std::endl;
//    std::cout << "Available Physical: " << MEMORY::getAvailablePhysical() << std::endl;
//    std::cout << "Consumed Physical: " << MEMORY::getConsumedPhysical() << std::endl;
//
//    std::cout << "Total Virtual: " << MEMORY::getTotalVirtual() << std::endl;
//    std::cout << "Available Virtual: " << MEMORY::getAvailableVirtual() << std::endl;
//    std::cout << "Consumed Virtual: " << MEMORY::getConsumedVirtual() << std::endl;

    std::cout << "Points in tree = " << Global::get().getTree()->getPointCount() << std::endl;

    List ret;

    // return subclusters in matrix: rows == subclusters, columns = centroid dimensions (//+N, +radius/diameter == clustersize )
    NumericMatrix subclusters_mat(getRowsCount(), getColsCount());
    fillReturnMatrixWithSubclusters(Global::get().getTree(), subclusters_mat.begin());
    ret["subclusters"] = subclusters_mat;

    NumericMatrix outliers_mat(Global::get().outliers.size(), getColsCount());
    fillReturnMatrixWithOutliers(Global::get().outliers, outliers_mat.begin());
    ret["outliers"] = outliers_mat;

//    std::cout << "Memory without tree = " << Global::get().getMemoryWithoutTree() << std::endl;
//    std::cout << "Memory NOW = " << MEMORY::getConsumedPhysical() << std::endl;
//    std::cout << "Memory MAX = " << Global::get().getMemoryMax() << std::endl;
//    std::cout << "Memory of tree NOW = " << MEMORY::getConsumedPhysical() - Global::get().getMemoryWithoutTree() << std::endl;
//    std::cout << "Memory of tree MAX = " << Global::get().getMemoryMax() - Global::get().getMemoryWithoutTree() << std::endl;

    return ret;
}
