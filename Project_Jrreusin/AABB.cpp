//
//  AABB.cpp
//  A5
//
//  Created by Joseph Reusing on 2018-07-22.
//  Copyright © 2018 none. All rights reserved.
//

#include "AABB.hpp"


Intersect AABB::IntersectAABB(const AABB& other) const
{
    //The distance between the AABB's on the X, Y, and Z axis.
    //Computed twice because there are two possible valid distances, depending
    //on the location of the AABB's.
    glm::vec3 distances1 = other.GetMinExtents() - m_maxExtents;
    glm::vec3 distances2 = m_minExtents - other.GetMaxExtents();
    
    //The correct distances will be whichever distance is larger for that
    //particular axis.
    glm::vec3 distances = glm::vec3(std::max(distances1.x, distances2.x),
                          std::max(distances1.y, distances2.y),
                          std::max(distances1.z, distances2.z));
    
    float maxDistance = std::max(distances.x, distances.y);
    maxDistance = std::max(maxDistance, distances.z);
    
    //If there is any distance between the two AABB's, then max distance will
    //be greather than or equal to 0. If there is distance between the two
    //AABBs, then they aren't intersecting.
    //
    //Therefore, if the AABBs are intersecting, then the distance between them
    //must be less than zero.
    return Intersect(maxDistance < 0, maxDistance);
}
