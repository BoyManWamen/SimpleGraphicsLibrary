#include "geometry.h"

vector3 cross(const vector3 &v1, const vector3 &v2) {
    return vector<3>{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}
