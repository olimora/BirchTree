//
// Created by omora on 2018-03-22.
//

#include "class_CF.h"
#include "utils_vector.h"
#include "algo_calculations.h"

CF::CF() {
    this->N = 0;
    this->LS = vec::zeros();
    this->SS = 0.;
}

CF::CF(int N, Vec LS, double SS) {
    this->N = N;
    this->LS = LS;
    this->SS = SS;
}

CF::CF(std::shared_ptr<CF> cf1, std::shared_ptr<CF> cf2) {
    this->N = cf1->N + cf2->N;
    this->LS = cf1->LS + cf2->LS;
    this->SS = cf1->SS + cf2->SS;
}

/// functions

Vec CF::getCentroid() {
    return calculateCentroid(this->LS, this->N);
}

double CF::getClusterSize() {
    Vec empty;
    return calculateClusterSize(shared_from_this(), empty);
}

double CF::getClusterSize(Vec point) {
    return calculateClusterSize(shared_from_this(), point);
}

void CF::addPoint(Vec point) {
    this->N += 1;
    this->LS += point;
    this->SS += vec::squaredSum(point);
}

int CF::getMemoryConsumed() {
    int sum = sizeof(this->N) + sizeof(this->LS) + (this->LS.size() * sizeof(this->LS[0])) + sizeof(this->SS) + sizeof(this);
    return sum;
}

//double CF::getRadius() {
//    return calculateRadius(this->N, this->LS, this->SS, this->getCentroid());
//}
//
//double CF::getRadiusWithPoint(Vec point) {
//    return calculateRadius(this->N + 1, this->LS + point, this->SS + vec::squaredSum(point), calculateCentroid(this->LS + point, this->N + 1));
//}

