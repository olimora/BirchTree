#ifndef UTILS_VECTOR_H
#define UTILS_VECTOR_H

#ifdef _MSC_VER
#pragma once
#endif

#include <valarray>

using Vec = std::valarray<double>;

namespace VEC {
    Vec zeros();

    double squaredSum(Vec point);
}


#endif