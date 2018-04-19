//
// Created by omora on 2018-04-11.
//

#include "utils_return_matrix.h"
#include "include_global.h"

const int EXTRA_COLS_COUNT = 2; // N (number of points in subcluster), and cluster size (radius or diameter of subcluster)
int ROWS = 0;
int COLS = 0;
int* I_ROW;


int getRowsCount() {
    return Global::get().getTree()->subcluster_count;
}
int getColsCount() {
    return Global::get().getDimensions() + EXTRA_COLS_COUNT;
}

int getIndexInMatrix(int row, int col, int rows) {
    return col * rows + row;
}

void traverseTheTree(std::shared_ptr<BNode> node, double *matrix);
void fillMatrixRow(std::shared_ptr<CF> cf, double *matrix);

void fillReturnMatrixWithSubclusters(std::shared_ptr<CFTree> tree, double* matrix) {
    ROWS = getRowsCount();
    COLS = getColsCount();
    int i_row_og = 0;
    I_ROW = &i_row_og; // pass i_row to rec func as reference, increase value after row insertion

    // recursive call on tree->root to get to LNodes->subclusters
    // for each subcluster add it to matrix row
    traverseTheTree(tree->root, matrix);

}

void traverseTheTree(std::shared_ptr<BNode> node, double *matrix) {
    if (!node->isLeafNode()) {
        std::shared_ptr<NLNode> nlnode = std::dynamic_pointer_cast<NLNode>(node);
        for (auto entry : nlnode->entries) {
            traverseTheTree(entry, matrix);
        }
    } else { // leaf node
        std::shared_ptr<LNode> lnode = std::dynamic_pointer_cast<LNode>(node);
        for (auto entry : lnode->entries) {
            fillMatrixRow(entry, matrix);
        }
    }
}

void fillMatrixRow(std::shared_ptr<CF> cf, double *matrix) {
    Vec fill_in = cf->getCentroid();
    for (unsigned int col = 0; col < fill_in.size(); col++) {
        // add it to proper place in matrix
        // it goes by columns ... so can not put it in as row (continuing values)
        // compute proper index in matrix for every value from ROWS, COLS, I_ROW, col
        // matrix[index] = fill_in[col]
        matrix[getIndexInMatrix(*I_ROW, col, ROWS)] = fill_in[col];
    }

    // N
    matrix[getIndexInMatrix(*I_ROW, COLS-2, ROWS)] = cf->N;
    // cluster size
    matrix[getIndexInMatrix(*I_ROW, COLS-1, ROWS)] = cf->getClusterSize();

    (*I_ROW)++; //increment global row matrix index
}

void fillReturnMatrixWithOutliers(std::vector<std::shared_ptr<CF>> outliers, double* matrix) {
    ROWS = outliers.size();
    COLS = getColsCount();
    int i_row = 0;
    for (unsigned int i = 0; i < outliers.size(); i++) {
        Vec fill_in = outliers[i]->getCentroid();
        for (unsigned int col = 0; col < fill_in.size(); col++) {
            // add it to proper place in matrix
            // it goes by columns ... so can not put it in as row (continuing values)
            // compute proper index in matrix for every value from ROWS, COLS, I_ROW, col
            // matrix[index] = fill_in[col]
            matrix[getIndexInMatrix(i_row, col, ROWS)] = fill_in[col];
        }
        // N
        matrix[getIndexInMatrix(i_row, COLS-2, ROWS)] = outliers[i]->N;
        // cluster size
        matrix[getIndexInMatrix(i_row, COLS-1, ROWS)] = outliers[i]->getClusterSize();

        i_row++; //increment global row matrix index
    }
    std::cout << "end" << std::endl;
}