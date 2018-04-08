R -q -e "setwd('..'); Rcpp::compileAttributes('BirchTree');"
R CMD INSTALL '../BirchTree' -c --no-multiarch
pause