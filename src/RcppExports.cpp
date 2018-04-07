// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// buildTree
void buildTree(NumericMatrix data, int BF_B, int BF_L, double threshold_T, std::string distance_metric, std::string cluster_size_metric);
RcppExport SEXP _BirchTree_buildTree(SEXP dataSEXP, SEXP BF_BSEXP, SEXP BF_LSEXP, SEXP threshold_TSEXP, SEXP distance_metricSEXP, SEXP cluster_size_metricSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type data(dataSEXP);
    Rcpp::traits::input_parameter< int >::type BF_B(BF_BSEXP);
    Rcpp::traits::input_parameter< int >::type BF_L(BF_LSEXP);
    Rcpp::traits::input_parameter< double >::type threshold_T(threshold_TSEXP);
    Rcpp::traits::input_parameter< std::string >::type distance_metric(distance_metricSEXP);
    Rcpp::traits::input_parameter< std::string >::type cluster_size_metric(cluster_size_metricSEXP);
    buildTree(data, BF_B, BF_L, threshold_T, distance_metric, cluster_size_metric);
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_BirchTree_buildTree", (DL_FUNC) &_BirchTree_buildTree, 6},
    {NULL, NULL, 0}
};

RcppExport void R_init_BirchTree(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
