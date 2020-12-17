#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "scene_lua.hpp"
#include "Shader.hpp"
#include "Asteroid.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>


#include <memory>
#include <imgui/imgui.h>
#include <iostream>

#include <stack>          // std::stack
#include <vector>         // std::vector

#include "lodepng/lodepng.h"
#include "Shader.hpp"
#include "AABB.hpp"


struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

struct Particle {
    glm::vec3 position, velocity;
    glm::vec4 color;
    float life;
    
    Particle()
    : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
};


class A5 : public CS488Window {
public:
	A5(const std::string & luaSceneFile);
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();
	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderArcCircle();

	LightSource m_light; // main light

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry data:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
    
    // A3 Added Fields and Functions
    void renderSceneHelper(const SceneNode *node, std::vector<glm::mat4> &t_stack);
    SceneNode* getNodeByID(unsigned int id, SceneNode &node);
    SceneNode* getParentJointNode(unsigned int id, SceneNode &node, SceneNode *joint_node);
    SceneNode* getParentNode(unsigned int id, SceneNode &node);
    void rotateSelected(float angle, SceneNode *node);
    
    // Option Flags
    bool circle = true;
    bool zbuffer = true;
    bool bfcull = false;
    bool ffcull = false;
    
    bool action = false;
    bool do_picking = false; // picking mode is off at beginning
    
    // interaction mode Flag
    int i_mode = 0; // 0 - position/orientation, 1 - joints
    
    glm::mat4 offset_transform;
    glm::mat4 root_rotation;
    
    // Mouse Position Values
    float mouse_x = 0;
    float mouse_y = 0;
    glm::vec2 mouse_start;
    
    /* --------------------------------------------------
     --
     --   A5 Stuff pew pew YEEE BOI
     --
     -------------------------------------------------*/
     void initModel();
    unsigned int loadCubemap();
    unsigned int loadJPEGTexture(char const * path);
    
    void camShipTranslations(); // velocity translations
    void camShipRotations();    // view & ship rotations
    void updateYawPitchValues();  // update pitch/yaw values
    void offsetTransformations(); // set final offset transformations
    void asteroidTransformations();
    void handleCollisions();
   
    void renderSkyBox();
    void renderCube(glm::mat4 trans, glm::vec3 pos);
    void renderAsteroids();
    void renderAABBs();
    
    float randomFloat(float a, float b);
    
    //-- GL resources for skybox/reflective cubemap------|
    GLuint vbo_cubemap; // currently not used
    GLuint vao_cubemap; // same
    Shader shader_cubemap;
    unsigned int cubemap_texture;
    GLuint vbo_skybox;
    GLuint vao_skybox;
    Shader shader_skybox;
    bool skybox = true;
    // ---------------------------------------------------//
    
    // Normal/Bump Mapping &  Obstacles -------------------|
    Shader normal_map_shader;
    unsigned int ast_norm_texture;
    unsigned int ast_bump_texture;
    unsigned int ast_diff_texture;
    glm::vec3 lightPos;
    std::vector<Asteroid> asteroids;
    GLuint vao_cube;
    GLuint vbo_cube;
    GLuint vao_polyhedron;
    GLuint vbo_polyhedron;
    int cube_vertices = 0;
    int polyhedron_vertices = 0;
    bool bumpmap = true;
    // ---------------------------------------------------//
    
    // Model matrices--------------------------------------|
    glm::mat4 m_model;
    glm::mat4 m_perspective;
    glm::mat4 m_view;
    // ---------------------------------------------------//
    
    // CAMERA STUFF ---------------------------------------|
    glm::vec3 camera_pos;   // camera position (eye)
    glm::vec3 camera_up;    // cam up vector
    glm::vec3 camera_right; // cam right vector
    // ---------------------------------------------------//
    
    // SPACESHIP STUFF ------------------------------------|
    glm::vec3 ship_pos;   // camera lookat position
    glm::vec3 ship_aim;   // ship lookat position
    glm::vec3 ship_right; // cam right vector
    glm::vec3 ship_up;    // ship vector up
    
    // input variables -------
    bool gas_flag = false;
    float ship_velocity = 0.0f;
    bool up_flag = false;
    bool down_flag = false;
    bool right_flag = false;
    bool left_flag = false;
    float pitch_val = 0.0f; // pitch scale
    float yaw_val = 0.0f;
    bool debug_flag = false;
    // ---------------------------------------------------//
    
    // AABB STUFF ------------------------------------|
    GLuint vao_aabb;
    GLuint vbo_aabb;
    AABB ship_aabb;       // used for collision detection
    Shader plain_shader;
    // TODO
    
    // Particle System ------------------------------------|
    GLuint vbo_particle; 
    GLuint vao_particle;
    Shader particle_shader;
    unsigned int particle_texture;
    GLuint last_particle = 0;
    GLuint max_particles = 500;
    std::vector<Particle> particles;
    int amount = 50;
    
    void initParticles();
    void updateParticles();
    void respawnParticle(Particle &particle, glm::vec3 origin);
    void renderParticles();
    GLuint firstUnusedParticle();
    unsigned int loadPNGTexture(char const * path);
    double last_time;
    double delta_time;
    
};
