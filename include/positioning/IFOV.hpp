#ifndef IFON
#define IFON

#include <cmath>
#include <dataStructures/vectors.hpp>

/*
 * Checks if an Object is infront/in FOV of another Object (This Object)
 * pVectDir -> Vector in which the this Object watch
 * pVectDirToObj -> Vector from this Object to the checked Object
 * The higher the tolerance the smaller the field of view, 0.45 would mean FOV of 45° above and below  the pVectDir!
*/
namespace JFLX::positioning {

inline bool isInFOV(JFLX::Vector2<double> pVectDir, JFLX::Vector2<double> pVectDirToObj, double tolerance = 0.0) {
    return (pVectDir.dot(pVectDirToObj) > tolerance);
}

} // namespace JFLX::positioning

#endif
