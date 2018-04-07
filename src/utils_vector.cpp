#include "utils_vector.h"
#include "include_global.h"

namespace vec {
    Vec zeros() {
        return Vec(Global::get().getDimensions());
    }

    double squaredSum(Vec vec) {
        Vec ret = std::pow(vec, 2.);
        return ret.sum();
    }
}
