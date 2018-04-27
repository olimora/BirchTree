//
// Created by omora on 2018-03-26.
//


#include "algo_distance.h"
#include <stdexcept>
#include "include_global.h"
#include <valarray>

double distance(Vec A, Vec B) {
    if (A.size() != B.size()) {
        std::cout << "A = " << A.size() << ", B = " << B.size() << std::endl;
        throw std::invalid_argument( "Dist D0: Vector size does not match." );
    }
    return Global::get().calculateDistance(A, B);
}

distanceFPtr assignDistanceMetric(std::string metric) {
    if (metric.compare(std::string("euclid")) == 0) {   // for Euclidean dist - D0
        return dist_D0;
    } else if (metric.compare(std::string("manhattan")) == 0) { // for Manhattan dist - D1
        return dist_D1;
    } else {   // else default
        return dist_D0;
    }
}

double dist_D0(Vec A, Vec B) {
    return std::sqrt(pow(A-B, 2).sum());
}

double dist_D1(Vec A, Vec B) {
    return std::abs(A-B).sum();
}

