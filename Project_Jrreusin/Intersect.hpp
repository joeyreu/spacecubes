//
//  Intersect.hpp
//  A3
//
//  Created by Joseph Reusing on 2018-07-22.
//  Copyright © 2018 none. All rights reserved.
//

#pragma once

/**
 * The Intersect class stores info about two intersecting objects.
 */
class Intersect {
public:
    /**
     * Creates Intersect Data in a usable state.
     *
     * @param doesIntersect Whether or not the objects are intersecting.
     * @param distance      The distance between the two objects
     */
    
    Intersect(const bool doesIntersect, const float distance) :
    m_doesIntersect(doesIntersect),
    m_distance(distance) {}
    
    /** Basic getter for m_doesIntersect */
    inline bool GetDoesIntersect() const { return m_doesIntersect; }
    /** Basic getter for m_distance */
    inline float GetDistance()     const { return m_distance; }
private:
    /** Whether or not the objects are intersecting */
    const bool  m_doesIntersect;
    /** The distance between the two objects */
    const float m_distance;
};
