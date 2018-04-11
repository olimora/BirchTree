#include "utils_vector.h"
#include "include_global.h"

namespace VEC {
    Vec zeros() {
        return Vec(Global::get().getDimensions());
    }

    double squaredSum(Vec vec) {
        return pow(vec, 2.).sum();
    }
}
