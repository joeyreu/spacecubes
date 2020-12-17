#pragma once

#include<vector>
#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <iostream>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "AABB.hpp"

class Asteroid {
public:
    GLuint asteroid_vao;    // Vertex Array Object
    GLuint asteroid_vbo;    // Vertex Buffer Object
    
    glm::mat4 transform;     // transformation (rotation)
    glm::vec3 pos;           // Position (translation)
    glm::vec3 direction;     // direction asteroid is travelling
    float speed;
    bool alive;
    AABB aabb;
    int type = 0;
    
    Asteroid(glm::vec3 pos);      // Constructor
    virtual ~Asteroid();          // Destructor
    virtual void init();
};
