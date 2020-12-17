//
//  Asteroid.cpp
//  A5
//
//  Created by Joseph Reusing on 2018-07-21.
//  Copyright © 2018 none. All rights reserved.
//

#include "Asteroid.hpp"

using namespace glm;
using namespace std;


//----------------------------------------------------------------------------------------
// Constructor
Asteroid::Asteroid(glm::vec3 pos)
:  pos(pos), alive(true),
   aabb(AABB(vec3(-0.5f, -0.5f, -0.5), vec3(0.5f, 0.5f, 0.5))) // default 1x1 cube AABB
{
    transform = mat4(1.0f);
    speed = 0;
}


//----------------------------------------------------------------------------------------
// Destructor
Asteroid::~Asteroid()
{
}


//----------------------------------------------------------------------------------------
//--
void Asteroid::init()
{
}

