//
//  AABB.hpp
//  A5
//
//  Created by Joseph Reusing on 2018-07-22.
//  Copyright © 2018 none. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <algorithm> 
#include "Intersect.hpp"

class AABB
{
public:
    /**
     * Creates an AABB in a usable state.
     *
     * @param minExtents The corner of the AABB with the smallest coordinates.
     * @param maxExtents The corner of the AABB with the largest coordinates.
     */
    AABB(glm::vec3 minExtents, glm::vec3 maxExtents) :
    m_minExtents(minExtents),
    m_maxExtents(maxExtents) {}
    
    /**
     * Computes information about if this AABB intersects another AABB.
     *
     * @param other The AABB that's being tested for intersection with this
     *                AABB.
     */
    Intersect IntersectAABB(const AABB& other) const;
    
    /** Basic getter for the min extents */
    inline glm::vec3 GetMinExtents() const { return m_minExtents; }
    /** Basic getter for the max extents */
    inline glm::vec3 GetMaxExtents() const { return m_maxExtents; }

    /** The corner of the AABB with the smallest coordinates */
    glm::vec3 m_minExtents;
    /** The corner of the AABB with the largest coordinates */
    glm::vec3 m_maxExtents;
};
