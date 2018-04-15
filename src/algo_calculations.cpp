

#include "algo_calculations.h"
#include "include_global.h"


//double calculateClusterSize(std::shared_ptr<CF> cf) {
//    return Global::get().calculateClusterSize(cf);
//}

double calculateClusterSize(std::shared_ptr<CF> cf, Vec point) {
    return Global::get().calculateClusterSize(cf, point);
}

clusterSizeFPtr assignClusterSizeMetric(std::string metric) {
    if (metric.compare(std::string("radius")) == 0) {           // for radius
        return calculateRadius;
    } else if (metric.compare(std::string("diameter")) == 0) {  // for diameter
        return calculateDiameter;
    } else {                                                    // else default
        return calculateRadius;
    }
}

Vec calculateCentroid(Vec LS, int N) {
    return LS / double(N);
}

//double calculateRadius(std::shared_ptr<CF> cf) {
//    if (cf->N < 2) return 0;
//    Vec C = calculateCentroid(cf->LS, cf->N);
//    return std::sqrt((cf->SS - (2*(cf->LS*C).sum()) + (cf->N*(C*C).sum())) / cf->N);
//}

double calculateRadius(int N, Vec LS, double SS, Vec C) {
    if (N < 2) return 0;
    return std::sqrt((SS - (2*(LS*C).sum()) + (N*(C*C).sum())) / N);
}

double calculateRadius(std::shared_ptr<CF> cf, Vec point) {
    if (point.size() > 0) {
        return calculateRadius(cf->N +1 , cf->LS + point, cf->SS + VEC::squaredSum(point),
                               calculateCentroid(cf->LS + point, cf->N + 1));
    } else {
        return calculateRadius(cf->N, cf->LS, cf->SS, calculateCentroid(cf->LS, cf->N));
    }

}

double calculateDiameter(int N, Vec LS, double SS) {
    if (N < 2) return 0;
    return std::sqrt((2*(SS*N-(LS*LS).sum())) / (N*(N-1)));
}

double calculateDiameter(std::shared_ptr<CF> cf, Vec point) {
    if (point.size() > 0) {
        return calculateDiameter(cf->N +1 , cf->LS + point, cf->SS + VEC::squaredSum(point));
    } else {
        return calculateDiameter(cf->N, cf->LS, cf->SS);
    }
}

double calculateDensity(std::shared_ptr<CF> cf) {
    if (cf->getClusterSize() == 0) {
        return 0;
    } else {
        return cf->N / cf->getClusterSize();
    }
}
