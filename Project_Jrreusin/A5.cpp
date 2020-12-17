#include "A5.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;
using namespace std;

static bool show_gui = true;


const size_t CIRCLE_PTS = 48;

//// CPU representation of a particle
//struct Particle{
//    glm::vec3 pos, speed;
//    unsigned char r,g,b,a; // Color
//    float size, angle, weight;
//    float life; // Remaining life of the particle. if < 0 : dead and unused.
//    float cameradistance; // distance to the camera
//
//    bool operator<(const Particle& that) const {
//        // Sort in reverse order : far particles drawn first.
//        return this->cameradistance > that.cameradistance;
//    }
//};


const int max_particles = 100;
int particle_count = 0;
//----------------------------------------------------------------------------------------
// Constructor
A5::A5(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
      ship_aabb(AABB(vec3(-0.5f, -0.5f, -0.5), vec3(0.5f, 0.5f, 0.5)))
{
    
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5(){
}

//----------------------------------------------------------------------------------------
// Called once, after program is signaled to terminate.
void A5::cleanup(){
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
// TODO: Add other init values
void A5::init(){
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);
    
    // Generate vao vertex arrays
    glGenVertexArrays(1, &m_vao_arcCircle);
    glGenVertexArrays(1, &m_vao_meshData);
    glGenVertexArrays(1, &vao_cubemap);
    glGenVertexArrays(1, &vao_cube);
    glGenVertexArrays(1, &vao_skybox);
    glGenVertexArrays(1, &vao_aabb);
    

    // Build an compile shaders
    // ------------------------
	createShaderProgram();
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);
	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
            getAssetFilePath("polyhedron.obj")
	});
	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);
	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);
	mapVboDataToVertexShaderInputLocations();
    
    // init values
    camera_pos = vec3(0.0f, 0.0f, 0.0f);
    camera_up = vec3(0.0f, 1.0f, 0.0f);
    ship_pos = vec3(0.0f, 0.0f, -3.5f); // TODO: need to set to rootNode of spaceship
    ship_up = vec3(0.0f, 1.0f, 0.0f);
    ship_aim = vec3(0.0f, 0.0f, -4.0f);
    
    root_rotation = mat4(1.0f);
    root_rotation = glm::rotate(root_rotation,
                                float(glm::radians(180.0f)),
                                vec3(0.0f, 1.0f, 0.0f));

	initPerspectiveMatrix();
	initViewMatrix();
	initLightSources();
    initModel();
    
    // load textures
    // 0 : ???
    // 1 : cubemap_texture
    // 2 : ast_norm_texture
    // 3 : ast_diff_texture
    // ----------------------
    cubemap_texture = loadCubemap(); // Skybox Cubemap Texture
    ast_norm_texture = loadJPEGTexture("Assets/asteroid_norm_map.jpg"); // Asteroid Textures
    ast_diff_texture = loadJPEGTexture("Assets/asteroid_diffuse_map.jpg");
    ast_bump_texture = loadJPEGTexture("Assets/asteroid_bump_map.jpg");
    particle_texture = loadPNGTexture("Assets/yellowlight.png");
    // shader configurations
    // --------------------
    shader_cubemap.use();
    shader_cubemap.setInt("skybox", 0);
    
    shader_skybox.use();
    shader_skybox.setInt("skybox", 0);
    
    m_shader.enable();
    GLint location = m_shader.getUniformLocation("skybox");
    glUniform1i(location, 0);
    m_shader.disable();
    
    normal_map_shader.use();
    normal_map_shader.setInt("diffuseMap", 0);
    normal_map_shader.setInt("normalMap", 1);

    
    initParticles();
    
    
    // Spawn Asteroids
    // ---------------------
    Asteroid test_asteroid = Asteroid( vec3( 0.0f, 0.0f, -4.0f ) );
    asteroids.push_back(test_asteroid);
    for (int i = 0; i < 200; i ++ ) {
        // Generate random position...
        float x = randomFloat(ship_pos.x - 10, ship_pos.x + 40);
        float y = randomFloat(ship_pos.y - 10, ship_pos.y + 40);
        float z = randomFloat(ship_pos.z - 10, ship_pos.z + 40);
        Asteroid new_asteroid = Asteroid( vec3( x, y, z ) );
        
        // random size
        new_asteroid.transform = glm::scale(mat4(1.0f), vec3(randomFloat(1, 20)));
        
        // Add random rotation, direction, TODO: spin?
        x = randomFloat(0, 10);
        y = randomFloat(0, 10);
        z = randomFloat(0, 10);
        new_asteroid.transform = glm::rotate(new_asteroid.transform, x, vec3(1.0f, 0.0f, 0.0f));
        new_asteroid.transform = glm::rotate(new_asteroid.transform, y, vec3(0.0f, 1.0f, 0.0f));
        new_asteroid.transform = glm::rotate(new_asteroid.transform, z, vec3(0.0f, 0.0f, 1.0f));
        
        x = randomFloat(-1, 1);
        y = randomFloat(-1, 1);
        z = randomFloat(-1, 1);
        new_asteroid.direction = glm::normalize(vec3(x,y,z));
        new_asteroid.speed = randomFloat(0.01, 0.6);

        asteroids.push_back(new_asteroid);
    }
    
    last_time = glfwGetTime();
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A5::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
// build and compile shaders
void A5::createShaderProgram() {
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str());
	m_shader_arcCircle.link();
    
    shader_cubemap = Shader("Assets/cubemaps.vs", "Assets/cubemaps.fs");
    shader_skybox = Shader("Assets/skybox.vs", "Assets/skybox.fs");
    
    normal_map_shader = Shader("Assets/normal_map.vs", "Assets/normal_map.fs");
    particle_shader = Shader("Assets/particle.vs", "Assets/particle.fs");
    
    plain_shader = Shader("Assets/plain.vs", "Assets/plain.fs");
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots() {
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);
        
		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}
    
	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
    
    
    // Generate VBO and VAO to store the skybox
    {
        float cube_vertices[] = {
            // pos              // norms
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };
        float skybox_vertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            
            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
        // cube VAO
        glGenBuffers(1, &vbo_cubemap);
        glBindVertexArray(vao_cubemap);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cubemap);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // skybox VAO
        glGenBuffers( 1, &vbo_skybox);
        glBindVertexArray(vao_skybox);
        glBindBuffer( GL_ARRAY_BUFFER, vbo_skybox);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices),
                     &skybox_vertices, GL_STATIC_DRAW);
        // enable vertex shader input slots
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        CHECK_GL_ERRORS;
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }
    
    // Generate VBO and VAO to store the cuberoid
    {
        BatchInfoMap batchInfoMap;
        meshConsolidator.getBatchInfoMap(batchInfoMap);
        BatchInfo batchInfo = batchInfoMap["cube"];
        int numIndices = batchInfo.numIndices;
        int startIndex = batchInfo.startIndex;
        const float *vertex_positions = meshConsolidator.getVertexPositionDataPtr();
        const float *vertex_norms = meshConsolidator.getVertexNormalDataPtr();
        vector<float> arr;
    
        int num_triangles = 0;
        
        for (int i = startIndex; i < startIndex + numIndices*3 ; i += 9) {
            vec3 pos1 = make_vec3(&vertex_positions[i]);
            vec3 pos2 = make_vec3(&vertex_positions[i+3]);
            vec3 pos3 = make_vec3(&vertex_positions[i+6]);
            int j = i + (numIndices/9 + 2)*9; // index of sister triangle
            
            // TRIANGLE 1
            // -----------
            // positions
        
            float edge_len1 = glm::distance(pos1, pos2);
            float edge_len2 = glm::distance(pos2, pos3);
            float edge_len3 = glm::distance(pos3, pos1);
            if (edge_len1 > edge_len2 && edge_len1 > edge_len3) {
                pos1 = make_vec3(&vertex_positions[i]);
                pos2 = make_vec3(&vertex_positions[i+3]);
                pos3 = make_vec3(&vertex_positions[i+6]);
            } else if (edge_len2 > edge_len1 && edge_len2 > edge_len3) {
                pos1 = make_vec3(&vertex_positions[i+3]);
                pos2 = make_vec3(&vertex_positions[i]);
                pos3 = make_vec3(&vertex_positions[i+6]);
            } else {
                pos1 = make_vec3(&vertex_positions[i]);
                pos2 = make_vec3(&vertex_positions[i+3]);
                pos3 = make_vec3(&vertex_positions[i+6]);
            }
            
            // normals
            vec3 nm1 = make_vec3(&vertex_norms[i]);
            vec3 nm2 = make_vec3(&vertex_norms[i+3]);
            vec3 nm3 = make_vec3(&vertex_norms[i+6]);
            // texture coordinates (square)
            glm::vec2 uv1(0.0f, 1.0f);
            glm::vec2 uv2(0.0f, 0.0f);
            glm::vec2 uv3(1.0f, 0.0f);
            glm::vec2 uv4(1.0f, 1.0f);
            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent, bitangent;
    
            glm::vec3 edge1;
            glm::vec3 edge2;
            glm::vec2 deltaUV1;
            glm::vec2 deltaUV2;
            
            deltaUV1 = uv2 - uv1;
            deltaUV2 = uv3 - uv1;
            edge1 = pos2 - pos1;
            edge2 = pos3 - pos1;
    
            GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);
    
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);
            
            // Append the 3 triangle vertices
            arr.push_back(pos1.x); arr.push_back(pos1.y); arr.push_back(pos1.z);                // position
            arr.push_back(nm1.x); arr.push_back(nm1.y); arr.push_back(nm1.z);                   // norms
            arr.push_back(uv1.x); arr.push_back(uv1.y);                                         // uv
            arr.push_back(tangent.x); arr.push_back(tangent.y); arr.push_back(tangent.z);       // tangent
            arr.push_back(bitangent.x); arr.push_back(bitangent.y); arr.push_back(bitangent.z); // bitangent
            
            arr.push_back(pos2.x); arr.push_back(pos2.y); arr.push_back(pos2.z);
            arr.push_back(nm2.x); arr.push_back(nm2.y); arr.push_back(nm2.z);
            arr.push_back(uv2.x); arr.push_back(uv2.y);
            arr.push_back(tangent.x); arr.push_back(tangent.y); arr.push_back(tangent.z);
            arr.push_back(bitangent.x); arr.push_back(bitangent.y); arr.push_back(bitangent.z);
            
            arr.push_back(pos3.x); arr.push_back(pos3.y); arr.push_back(pos3.z);
            arr.push_back(nm3.x); arr.push_back(nm3.y); arr.push_back(nm3.z);
            arr.push_back(uv3.x); arr.push_back(uv3.y);
            arr.push_back(tangent.x); arr.push_back(tangent.y); arr.push_back(tangent.z);
            arr.push_back(bitangent.x); arr.push_back(bitangent.y); arr.push_back(bitangent.z);
            
            // TRIANGLE 2
            // -----------
            // positions
            if (i/9  < 6) {
                vec3 pos1test = make_vec3(&vertex_positions[j]);
                vec3 pos2test = make_vec3(&vertex_positions[j+3]);
                vec3 pos3test = make_vec3(&vertex_positions[j+6]);
                vec3 pos4;
                if (pos1test != pos1 && pos1test != pos2 && pos1test != pos3) {
                    pos4 = pos1test;
                } else if (pos2test != pos1 && pos2test != pos2 && pos2test != pos3) {
                    pos4 = pos2test;
                } else {
                    pos4 = pos3test;
                }
                
                edge1 = pos3 - pos1;
                edge2 = pos4 - pos1;
                deltaUV1 = uv3 - uv1;
                deltaUV2 = uv4 - uv1;
                
                f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
                
                tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                tangent = glm::normalize(tangent);
                
                bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                
                // Append the 3 triangle vertices
                arr.push_back(pos1.x); arr.push_back(pos1.y); arr.push_back(pos1.z);                // position
                arr.push_back(nm1.x); arr.push_back(nm1.y); arr.push_back(nm1.z);                   // norms
                arr.push_back(uv1.x); arr.push_back(uv1.y);                                         // uv
                arr.push_back(tangent.x); arr.push_back(tangent.y); arr.push_back(tangent.z);       // tangent
                arr.push_back(bitangent.x); arr.push_back(bitangent.y); arr.push_back(bitangent.z); // bitangent
                
                arr.push_back(pos3.x); arr.push_back(pos3.y); arr.push_back(pos3.z);
                arr.push_back(nm2.x); arr.push_back(nm2.y); arr.push_back(nm2.z);
                arr.push_back(uv3.x); arr.push_back(uv3.y);
                arr.push_back(tangent.x); arr.push_back(tangent.y); arr.push_back(tangent.z);
                arr.push_back(bitangent.x); arr.push_back(bitangent.y); arr.push_back(bitangent.z);
                
                arr.push_back(pos4.x); arr.push_back(pos4.y); arr.push_back(pos4.z);
                arr.push_back(nm3.x); arr.push_back(nm3.y); arr.push_back(nm3.z);
                arr.push_back(uv4.x); arr.push_back(uv4.y);
                arr.push_back(tangent.x); arr.push_back(tangent.y); arr.push_back(tangent.z);
                arr.push_back(bitangent.x); arr.push_back(bitangent.y); arr.push_back(bitangent.z);
            }
            
            num_triangles ++;
            cube_vertices += 3;
        }
        
        // configure plane VAO
        glGenBuffers(1, &vbo_cube);
        
        glBindVertexArray(vao_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arr.size(), &(arr[0]), GL_STATIC_DRAW);
        
        // enable vertex shader input slots
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    
    // Generate VBO and VAO to store the
    {
        float line_vertices[] = {
            0.5f, 0.5f, 0.5f,  // front top right
            0.5f, -0.5f, 0.5f, // front bot right
            
            0.5f, -0.5f, 0.5f,  // front bot right
            -0.5f, -0.5f, 0.5f, // front bot left
            
            -0.5f, -0.5f, 0.5f // front bot left
            -0.5f, 0.5f, 0.5f, // front top left
            
            -0.5f, 0.5f, 0.5f, // front top left
            0.5f, 0.5f, 0.5f,  // front top right
            
             -0.5f, 0.5f, 0.5f, // front top left
             -0.5, 0.5f, -0.5f, // back top left
            
            -0.5f, 0.5f, -0.5f, // back top left
            -0.5f, -0.5f, -0.5f, // back bot left
            
            -0.5f, -0.5f, -0.5f, // back bot left
            -0.5f, -0.5f, 0.5f,  // front bot left
            
            0.5f, 0.5f, 0.5f,  // front top right
            0.5f, 0.5f, -0.5,  // back top right
            
            0.5f, 0.5f, -0.5f,  // back top right
            0.5f, -0.5f, -0.5f,  // back bot right
            
            0.5f, -0.5f, -0.5f,  // back bot right
            0.5f, -0.5f, 0.5f,  // front bot right
            
            
            0.5f, -0.5f, -0.5f,  // back bot right
            -0.5f, -0.5f, -0.5f, // back bot left
            
            0.5f, 0.5f, -0.5f,  // back top right
            -0.5f, 0.5f, -0.5f, // back top left
            
        };
        
        glGenBuffers(1, &vbo_aabb);

        glBindVertexArray(vao_aabb);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_aabb);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices, GL_STATIC_DRAW);
    }
    
    
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    // Bind VAo to record aabb data mapping
    glBindVertexArray(vao_aabb);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_aabb);
    glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
    
}

//----------------------------------------------------------------------------------------|
// Init Helper Functions
void A5::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perspective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 1000.0f);
}
// pitch and yaw, angles in radians (how much to rotate)
void A5::initViewMatrix() {
	m_view = glm::lookAt(camera_pos, ship_pos, camera_up);
}
//-----------
void A5::initLightSources() {
	// World-space position
    m_light.position = vec3(2.0f, 5.0f, 0.0f); // TODO: "relative to camera?"
	m_light.rgbIntensity = vec3(0.6f); // White light
    lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
}
void A5::initModel() {
    m_model = mat4();
}
// ---------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
        //-- Set Perpsective matrix uniform for the scene:
        GLint location = m_shader.getUniformLocation("Perspective");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
        location = m_shader.getUniformLocation("cameraPos");
        glUniform3f(location, camera_pos.x, camera_pos.y, camera_pos.z);
        CHECK_GL_ERRORS;
        
        location = m_shader.getUniformLocation("picking");
        glUniform1i( location, do_picking ? 1 : 0 );

        if (! do_picking) {
            //-- Set LightSource uniform for the scene:
            {
                location = m_shader.getUniformLocation("light.position");
                glUniform3fv(location, 1, value_ptr(m_light.position));
                location = m_shader.getUniformLocation("light.rgbIntensity");
                glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
                CHECK_GL_ERRORS;
            }
            
            //-- Set background light ambient intensity
            {
                location = m_shader.getUniformLocation("ambientIntensity");
                vec3 ambientIntensity(0.1f);
                glUniform3fv(location, 1, value_ptr(ambientIntensity));
                CHECK_GL_ERRORS;
            }
        }
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic() {
	// Place per frame, application logic here ...
    double current_time = glfwGetTime();
    delta_time = current_time - last_time;
    last_time = current_time;
	uploadCommonSceneUniforms();
    
    // update positions
    camShipTranslations();     // Translates based on velocity
    camShipRotations();        // apply rotations around aim to the camera and ship
    asteroidTransformations(); // Transforms asteroids
//    updateParticles();
    
    // update yaw and pitch values
    updateYawPitchValues();
    
    // offset translations (based on rotations)
    offsetTransformations();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic() {
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Space Wanderer", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
		// Add more gui elements here here ...
        if(ImGui::BeginMainMenuBar()) {
            // -- OPTIONS MENU --
            if(ImGui::BeginMenu("Options")) {
                if (ImGui::MenuItem("Bump Map", "M", &bumpmap, true)) {
                }
                if (ImGui::MenuItem("Skybox", "S", &skybox, true)) {
                }
                if (ImGui::MenuItem("Z-buffer", "Z", &zbuffer, true)) {
                }
                if (ImGui::MenuItem("Backface culling", "B", &bfcull, true)) {
                }
                if (ImGui::MenuItem("Frontface culling", "F", &ffcull, true)) {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    
        vec3 real_ship_pos = vec3(offset_transform * vec4(ship_pos, 1.0f));
        ImGui::Text("    Speed: %.2f km/h", float(ship_velocity/4)*5000 );
        ImGui::Text("Framerate: %.2f FPS", ImGui::GetIO().Framerate );
    
        // Create Button, and check if it was clicked:
        if( ImGui::Button( "Toggle Debug Mode " ) ) {
            debug_flag = !debug_flag;
        }
    
        if(debug_flag) {
            ImGui::Text("Debugging: ");
            ImGui::Text("          x     y     z  ");
            ImGui::Text("     ( %.2f, %.2f, %.2f ) - location ", ship_pos.x, ship_pos.y, ship_pos.z);
            ImGui::Text("     ( %.2f, %.2f, %.2f ) - location after offset", real_ship_pos.x, real_ship_pos.y, real_ship_pos.z);
            ImGui::Text("     ( %.2f, %.2f, %.2f ) - ship aim location", ship_aim.x, ship_aim.y, ship_aim.z);
            ImGui::Text("     ( %.2f, %.2f, %.2f ) - camera location", camera_pos.x, camera_pos.y, camera_pos.z);
            ImGui::Text("     Pitch: %.5f   |    Yaw: %.5f", pitch_val, yaw_val);
            if (gas_flag) {
                ImGui::Text("     Ship Velocity: %.5f (+)", ship_velocity);
            } else {
                ImGui::Text("     Ship Velocity: %.5f ", ship_velocity);
            }
        }
    
    

	ImGui::End();
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw() {
    if (zbuffer)
        glEnable( GL_DEPTH_TEST );
    if (bfcull || ffcull)
        glEnable(GL_CULL_FACE);
    if (bfcull && !ffcull)
        glCullFace(GL_BACK);
    else if (!bfcull && ffcull)
        glCullFace(GL_FRONT);
    else if (bfcull && ffcull)
        glCullFace(GL_FRONT_AND_BACK);
    
    if (circle)
        renderArcCircle();
    
    // renders scene
    renderSceneGraph(*m_rootNode);      // the spaceship
    renderAsteroids();                  // Asteroids (cubes for now)
    
//    renderParticles();
//    renderAABBs();                      // AABBs
    
    
    renderSkyBox();                     // lastly render the skybox (and reflective cube)
    
    if (bfcull || ffcull)
        glDisable(GL_CULL_FACE);
    if (zbuffer)
        glDisable( GL_DEPTH_TEST );
}
//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
                                 const ShaderProgram & shader,
                                 const GeometryNode & node,
                                 const glm::mat4 & viewMatrix,
                                 std::vector<glm::mat4> &t_vec,
                                 bool do_picking
                                 ) {
    
    shader.enable();
    {
        //-- Set ModelView matrix:
        GLint location = shader.getUniformLocation("ModelView");
        
        //<-- Hierarchial Transformations
        mat4 translations;
        for(vector<mat4>::iterator it = t_vec.begin(); it != t_vec.end(); ++it) {
            translations *= *it;
        }
        //-->
        
        mat4 modelView = viewMatrix * translations; //* = viewMatrix
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;
        
        if (do_picking) {
            float r = float(node.m_nodeId&0xff) / 255.0f;
            float g = float((node.m_nodeId>>8)&0xff) / 255.0f;
            float b = float((node.m_nodeId>>16)&0xff) / 255.0f;
            
            location = shader.getUniformLocation("material.kd");
            glUniform3f( location, r, g, b );
            CHECK_GL_ERRORS;
        } else {
            //-- Set NormMatrix:
            location = shader.getUniformLocation("NormalMatrix");
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;
            
            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            vec3 kd = node.material.kd;
            if (node.isSelected)
                kd = vec3(1.0, 1.0, 1.0);
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
            
            location = shader.getUniformLocation("material.ks");
            vec3 ks = node.material.ks;
            if (node.isSelected)
                ks = vec3(1.0, 1.0, 1.0);
            glUniform3fv(location, 1, value_ptr(ks));
            CHECK_GL_ERRORS;
            
            location = shader.getUniformLocation("material.shininess");
            if (node.isSelected)
                glUniform1f(location, 500);
            else
                glUniform1f(location, node.material.shininess);
            CHECK_GL_ERRORS;
        }
    }
    shader.disable();
    
}

//---------------------------
void A5::renderSceneGraph(const SceneNode & root) {
    //cout << "---- Begin Render Scene Graph ----" << endl;

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);
    
    // initialize transformation stack
    vector<mat4> t_vec;
    
    mat4 temp = m_rootNode->trans;
    m_rootNode->trans = glm::translate(ship_pos); // move to ship position
    m_rootNode->trans = m_rootNode->trans * root_rotation; // apply rotation from yaw/pitch
    m_rootNode->trans = offset_transform * m_rootNode->trans; // apply translate/rotation for offsets
    
    renderSceneHelper(&root, t_vec);
    
    m_rootNode->trans = temp;
    
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
    
    //cout << "---- End Render Scene Graph ----" << endl;
}
//---------------------------
void A5::renderSceneHelper(const SceneNode *node, std::vector<glm::mat4> &t_vec)
{
    //cout << "RenderSceneHelper node: " << node->m_name << endl;
    
    // push transformation onto vector
    t_vec.push_back(node->trans);
    
    for (const SceneNode *child_node: node->children) {
        renderSceneHelper(child_node, t_vec);
    }
    
    // Render the node if it is a geometry node
    if (node->m_nodeType == NodeType::GeometryNode) {
        
        const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
        updateShaderUniforms(m_shader, * geometryNode, m_view, t_vec, do_picking);
        
        // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
        BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
        
        //-- Now render the mesh:
        m_shader.enable();
        glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
        m_shader.disable();
    }
    
    // pop transformation from vector
    t_vec.pop_back();
}
//------------------------
// Draw the trackball circle.
void A5::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.01/aspect, 0.01, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.01, 0.01*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}
/* --------------------------------------------------------------------------------------- */


//----------------------------------------------------------------------------------------
/*
 * Event handlers.
 * A5::rotateSelected - Handles cursor entering the window area events.
 * A5::mouseMoveEvent - Handles mouse cursor movement events.
 * A5::mouseButtonInputEvent - Handles mouse button events.
 * A5::mouseScrollEvent - Handles mouse scroll wheel events.
 * A5::windowResizeEvent - Handles window resize events.
 * A5::keyInputEvent - Handles key input events.
 */
bool A5::cursorEnterWindowEvent (
                                 int entered
                                 ) {
    bool eventHandled(false);
    
    // Fill in with event handling code...
    
    return eventHandled;
}
// ---------------------------------------
bool A5::mouseMoveEvent (
                         double xPos,
                         double yPos
                         ) {
    bool eventHandled(false);
    
    mouse_x = xPos;
    mouse_y = yPos;
    
    // --- HANDLE INTERACTION HERE ---
    if (!ImGui::IsMouseHoveringAnyWindow()) {
    }
    
    return eventHandled;
}
// ---------------------------------------
bool A5::mouseButtonInputEvent (
                                int button,
                                int actions,
                                int mods
                                ) {
    bool eventHandled(false);
    // Fill in with event handling code...
    // --- INTERACTION BEGINS HERE ---
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        
    }
    return eventHandled;
}
// ---------------------------------------
bool A5::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}
// ---------------------------------------
bool A5::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}
// ---------------------------------------
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);
    
    if( action == GLFW_REPEAT || action == GLFW_PRESS ) {
        if (key == GLFW_KEY_UP) {
            up_flag = true;
        }
        if (key == GLFW_KEY_DOWN) {
            down_flag = true;
        }
        if(key == GLFW_KEY_LEFT) {
            left_flag = true;
        }
        if(key == GLFW_KEY_RIGHT) {
            right_flag = true;
        }
        if(key == GLFW_KEY_SPACE) {
            gas_flag = true;
        }
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_UP) {
            up_flag = false;
        }
        if (key == GLFW_KEY_DOWN) {
            down_flag = false;
        }
        if(key == GLFW_KEY_LEFT) {
            left_flag = false;
        }
        if(key == GLFW_KEY_RIGHT) {
            right_flag = false;
        }
        if(key == GLFW_KEY_SPACE) {
            gas_flag = false;
        }
    }
	// Fill in with event handling code...

	return eventHandled;
}
/* --------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------
/*
 * A3 Helper functions
 */
void A5::rotateSelected(float angle, SceneNode* node) {
    if (node->m_nodeType == NodeType::JointNode && node->isSelected) {
        JointNode * jointNode = static_cast<JointNode *>(node);
        jointNode->rotatePicker(angle);
    }
    for (SceneNode *child_node: node->children) {
        rotateSelected(angle, child_node);
    }
}

// ---------------------------------------
SceneNode* A5::getNodeByID(unsigned int id, SceneNode &node) {
    if (node.m_nodeId == id) {
        return &node;
    }
    for (SceneNode *child_node: node.children) {
        SceneNode* test_node = getNodeByID(id, *child_node);
        if (test_node != nullptr) {
            return test_node;
        }
    }
    return nullptr;
}
// ---------------------------------------
SceneNode* A5::getParentJointNode(unsigned int id, SceneNode &node, SceneNode *joint_node) {
    if (node.m_nodeId == id) {
        return joint_node;
    }
    if (node.m_nodeType == NodeType::JointNode) {
        joint_node = &node;
    }
    for (SceneNode *child_node: node.children) {
        SceneNode* test_node = getParentJointNode(id, *child_node, joint_node);
        if (test_node != nullptr) {
            return test_node;
        }
    }
    return nullptr;
}
// ---------------------------------------
SceneNode* A5::getParentNode(unsigned int id, SceneNode &node) {
    if (node.m_nodeId == id) {
        return nullptr;
    }
    for (SceneNode *child_node: node.children) {
        if (child_node->m_nodeId == id) {
            return &node;
        } else {
            SceneNode* test = getParentNode(id, *child_node);
            if (test != nullptr) {
                return test;
            }
        }
    }
    return nullptr;
}
/* --------------------------------------------------------------------------------------- */


//----------------------------------------------------------------------------------------
/*
 * A5 Helper functions
 */
unsigned int A5::loadCubemap() {
//    std::vector<std::string> faces = {
//        "Assets/blue/bkg1_right.png",
//        "Assets/blue/bkg1_left.png",
//        "Assets/blue/bkg1_top.png",
//        "Assets/blue/bkg1_bot.png",
//        "Assets/blue/bkg1_front.png",
//        "Assets/blue/bkg1_back.png"
//    };
    std::vector<std::string> faces = {
        "Assets/lightblue/right.png",
        "Assets/lightblue/left.png",
        "Assets/lightblue/top.png",
        "Assets/lightblue/bot.png",
        "Assets/lightblue/front.png",
        "Assets/lightblue/back.png"
    };
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    for (unsigned int i = 0; i < faces.size(); i++) {
        std::vector<unsigned char> image;
        unsigned int w, h;
        
        try {
            int errorCode = lodepng::decode(image, w, h, faces[i]);
            
            if (errorCode != 0) {
                throw std:: runtime_error("lodepng::decode failed with error: " + std::to_string(errorCode));
            }
        } catch (const std::exception &ex) {
            std::cerr << ex.what() << "\nFatal error" << std:: endl;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(image[0]));
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}
// utility function for loading a 2D texture from file
unsigned int A5::loadJPEGTexture(char const * path){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

unsigned int A5::loadPNGTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    std::vector<unsigned char> image;
    unsigned int w, h;
    
    try {
        int errorCode = lodepng::decode(image, w, h, path);
        
        if (errorCode != 0) {
            throw std:: runtime_error("lodepng::decode failed with error: " + std::to_string(errorCode));
        }
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\nFatal error" << std:: endl;
    }
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textureID);
    CHECK_GL_ERRORS;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(image[0]));
    CHECK_GL_ERRORS;
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECK_GL_ERRORS;
//    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &(image[0]));
    
    
    return textureID;
}

// ---------------------------------------
void A5::renderSkyBox() {
    // draw scene as normal
    shader_cubemap.use();
    glm::mat4 model = glm::translate(vec3(0.0f, 0.0f, -8.0f));
    glm::mat4 view = m_view;
    glm::mat4 projection = m_perspective;
    shader_cubemap.setMat4("model", model);
    shader_cubemap.setMat4("view", view);
    shader_cubemap.setMat4("projection", projection);
    shader_cubemap.setVec3("cameraPos", camera_pos);
    // cubes
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cubemap);
    glBindVertexArray(vao_cubemap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    if (skybox) {
        // draw skybox
        glDepthFunc(GL_LEQUAL);  // change depth function
        shader_skybox.use();
        
        // translate to camera pos
        mat4 modelView = m_view * glm::translate(camera_pos);
        shader_skybox.setMat4("ModelView", modelView);
        shader_skybox.setMat4("Perspective", m_perspective);
        
        // skybox cube
        glBindVertexArray(vao_skybox);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }
}
// ---------------------------------------
void A5::renderCube(mat4 trans, vec3 pos) {
    // configure view/projection matrices
    normal_map_shader.use();
    normal_map_shader.setMat4("projection", m_perspective);
    normal_map_shader.setMat4("view", m_view);
    
    // render normal-mapped cube
    mat4 model = mat4(1.0f);
    model = trans * glm::translate(model, pos);
    
    normal_map_shader.setMat4("model", model);
    normal_map_shader.setVec3("viewPos", camera_pos);
    normal_map_shader.setVec3("lightPos", m_light.position);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ast_diff_texture);
    if (bumpmap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ast_norm_texture);
    } else {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    CHECK_GL_ERRORS;
    
    glBindVertexArray(vao_cube);
    glDrawArrays(GL_TRIANGLES, 0 , cube_vertices);
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}
// ---------------------------------------
void A5::renderAsteroids() {
    for (int i = 0; i < asteroids.size(); i ++) {
        if (asteroids[i].alive == true) {
            if (asteroids[i].type == 0 ) {
                renderCube(asteroids[i].transform, asteroids[i].pos);
            }
        }
    }
}
// ---------------------------------------
void A5::updateYawPitchValues() {
    // update yaw and pitch vals
    if (left_flag) {
        if (yaw_val < 0.025f) {
            if (yaw_val > 0.0f && yaw_val < -0.06f) {
                yaw_val *= 1.05f;
            } else {
                yaw_val += 0.001f;
            }
        }
    } else if (right_flag) {
        if (yaw_val > -0.025f ) {
            if (yaw_val < 0.0f && yaw_val > 0.06f) {
                yaw_val *= 1.05f;
            } else {
                yaw_val -= 0.001f;
            }
        }
    } else {
        if(yaw_val < 0.00001f && yaw_val > -0.00001f) {
            yaw_val = 0.0f;
        }
        if (yaw_val > 0) {
            yaw_val *= 0.98f;
        } else if (yaw_val < 0) {
            yaw_val *= 0.98f;
        }
    }
    
    if (up_flag) {
        if (pitch_val > -0.025f ) {
            if (pitch_val < 0.0f && pitch_val > 0.06f) {
                pitch_val *= 1.05f;
            } else {
                pitch_val -= 0.001f;
            }
        }
    } else if (down_flag) {
        
        if (pitch_val < 0.025f) {
            if (pitch_val > 0.0f && pitch_val < -0.06f) {
                pitch_val *= 1.05f;
            } else {
                pitch_val += 0.001f;
            }
        }
    } else {
        if(pitch_val < 0.00001f && pitch_val > -0.00001f) {
            pitch_val = 0.0f;
        }
        if (pitch_val > 0) {
            pitch_val *= 0.98f;
        } else if (pitch_val < 0) {
            pitch_val *= 0.98f;
        }
    }
}
// ---------------------------------------
void A5::offsetTransformations() {
    offset_transform = mat4(1.0f);
    
    // translate ship forwards a bit based on velocity
    vec3 direction = glm::normalize(ship_aim - ship_pos) * 0.3f * ship_velocity;
    offset_transform = glm::translate(offset_transform, direction);
    
    // move ship slightly based on pitch and yaw (rate of rotation)
    vec3 ship_to_aim = ship_pos - ship_aim;
    direction =  glm::normalize(glm::cross(ship_to_aim, ship_up)) * -40.0f * yaw_val;
    offset_transform = glm::translate(offset_transform, direction);
    
    direction = glm::normalize(glm::cross(ship_right, ship_to_aim)) * 40.0f * pitch_val;
     offset_transform = glm::translate(offset_transform, direction);
}

// ---------------------------------------
void A5::camShipTranslations(){
    if (ship_velocity > 0.0f) {
        vec3 direction = glm::normalize(ship_aim - camera_pos) * 0.1f * ship_velocity;
        mat4 transform = glm::translate(direction);
        camera_pos = vec3( transform * vec4(camera_pos, 1));
        ship_pos = vec3( transform * vec4(ship_pos, 1));
        ship_aim = vec3( transform * vec4(ship_aim, 1));
    }
    if (gas_flag) {
        if (ship_velocity < 4.0f) {
            if (ship_velocity == 0)
                ship_velocity = 0.001f;
            if(ship_velocity < 0.08f)
                ship_velocity += 0.01;
            else
                ship_velocity *= 1.05f;
            ship_velocity = ship_velocity > 4.0f ? 4.0f: ship_velocity;
        }
    } else {
        if (ship_velocity < 0.00001f)
            ship_velocity = 0.0f;
        if (ship_velocity > 0 )
            ship_velocity *= 0.98f;
    }
}
// ---------------------------------------
void A5::camShipRotations() {
    // * MOVEMENT INPUT HANDLING
    // 1. update camera values
    vec3 cam_to_aim = camera_pos - ship_aim;
    vec3 ship_to_aim = ship_pos - ship_aim;
    
    // calc right and up vectors
    camera_right = glm::normalize(glm::cross(cam_to_aim, camera_up));
    camera_up = glm::normalize(glm::cross(camera_right, cam_to_aim));
    ship_right = glm::normalize(glm::cross(ship_to_aim, ship_up));
    ship_up = glm::normalize(glm::cross(ship_right, ship_to_aim));
    
    // 2. init yaw and pitch mat4
    mat4 yaw_rotmat = mat4(1.0f);
    mat4 pitch_rotmat = mat4(1.0f);
    mat4 yaw_ship = mat4(1.0f);
    mat4 pitch_ship = mat4(1.0f);
    
    if (pitch_val >= 0) {
        // rotate camera and ship up
        // 3. pitch calculation
        pitch_rotmat = glm::rotate(pitch_rotmat, pitch_val, camera_right);
        camera_pos = vec3((pitch_rotmat * vec4(cam_to_aim, 0.0f))) + ship_aim;
        pitch_ship = glm::rotate(pitch_ship, pitch_val, ship_right);
        ship_pos = vec3((pitch_ship * vec4(ship_to_aim, 0.0f))) + ship_aim;
        root_rotation = pitch_ship * root_rotation;
    } else {
        // rotate camera and ship down
        // 3. pitch calculation
        pitch_rotmat = glm::rotate(pitch_rotmat, pitch_val, camera_right);
        camera_pos = vec3((pitch_rotmat * vec4(cam_to_aim, 0.0f))) + ship_aim;
        pitch_ship = glm::rotate(pitch_ship, pitch_val, ship_right);
        ship_pos = vec3((pitch_ship * vec4(ship_to_aim, 0.0f))) + ship_aim;
        root_rotation = pitch_ship * root_rotation;
    }
    
    // 4. update camera/ship values
    cam_to_aim = camera_pos - ship_aim;
    camera_right = glm::normalize(glm::cross(cam_to_aim, camera_up));
    camera_up = glm::normalize(glm::cross(camera_right, cam_to_aim));
    ship_to_aim = ship_pos - ship_aim;
    ship_right = glm::normalize(glm::cross(ship_to_aim, ship_up));
    ship_up = glm::normalize(glm::cross(ship_right, ship_to_aim));
    
    if (yaw_val >= 0) {
        // 5. rotate camera and ship left
        yaw_rotmat = glm::rotate(yaw_rotmat, yaw_val, camera_up); // camera_up));
        camera_pos = vec3(((yaw_rotmat) * vec4(cam_to_aim, 0.0f))) + ship_aim;
        yaw_ship = glm::rotate(yaw_ship, yaw_val, ship_up);
        ship_pos = vec3(((yaw_ship) * vec4(ship_to_aim, 0.0f))) + ship_aim;
        root_rotation = yaw_ship * root_rotation ;
    } else {
        // 5. rotate camera and ship right
        yaw_rotmat = glm::rotate(yaw_rotmat, yaw_val, camera_up); // camera_up));
        camera_pos = vec3(((yaw_rotmat) * vec4(cam_to_aim, 0.0f))) + ship_aim;
        yaw_ship = glm::rotate(yaw_ship, yaw_val, ship_up);
        ship_pos = vec3(((yaw_ship) * vec4(ship_to_aim, 0.0f))) + ship_aim;
        root_rotation = yaw_ship * root_rotation;
    }
    
    // 6. update m_view
    m_view = glm::lookAt(camera_pos, ship_aim,
                         camera_up);
    
    // 7. update camera/ship values
    cam_to_aim = camera_pos - ship_aim;
    camera_right = glm::normalize(glm::cross(cam_to_aim, camera_up));
    camera_up = glm::normalize(glm::cross(camera_right, cam_to_aim));
    ship_to_aim = ship_pos - ship_aim;
    ship_right = glm::normalize(glm::cross(ship_to_aim, ship_up));
    ship_up = glm::normalize(glm::cross(ship_right, ship_to_aim));
}
// ---------------------------------------
float A5::randomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
// ---------------------------------------
void A5::asteroidTransformations() {
    for (int i = 0; i < asteroids.size(); i ++) {
        mat4 transform = glm::translate(asteroids[i].direction * 0.05f * asteroids[i].speed);
        asteroids[i].pos = vec3( transform * vec4(asteroids[i].pos, 1));
    }
}

void A5::renderAABBs() {
    plain_shader.use();
    BatchInfo batchInfo = m_batchInfoMap["cube"];
    // first render the ship aabb
    glBindVertexArray(vao_aabb);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_aabb);
    
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void A5::handleCollisions() {
    
}




//----------------------------------------------------------------------------------------
/*
 *  PARTICLE SYSTEM FUNCTIONS
 */
void A5::initParticles() {
    // Set up mesh and attribute properties
    GLfloat particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &vao_particle);
    glGenBuffers(1, &vbo_particle);
    glBindVertexArray(vao_particle);
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_particle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // Set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    // Create this->amount default particle instances
    for (GLuint i = 0; i < amount; ++i)
    particles.push_back(Particle());
}
void A5::updateParticles() {
    GLuint new_particles = 2;
    // Add new particles
    for (GLuint i = 0; i < new_particles; ++i) {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], ship_pos);
    }
    // Uupdate all particles
    for (GLuint i = 0; i < particles.size(); ++i)
    {
        Particle &p = particles[i];
        p.life -= delta_time; // reduce life
        if (p.life > 0.0f)
        {    // particle is alive, thus update
            p.position -= p.velocity * (float)delta_time;
            p.color.a -= delta_time * 2.5;
        }
    }
}
void A5::respawnParticle(Particle &particle, vec3 origin) {
    GLfloat random = ((rand() % 100) - 50) / 10.0f;
    GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
    particle.position = origin + random; // + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.0f;
    particle.velocity = glm::normalize(ship_aim - ship_pos) * 0.3f * ship_velocity * 0.1f;
}

// ---------------------------------------
void A5::renderParticles() {
    // Use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    particle_shader.use();
    for (Particle particle : particles)
    {
        if (particle.life > 0.0f)
        {
            particle_shader.setVec3("offset", particle.position);
            particle_shader.setVec4("color", particle.color);
            // **** bind particle texture here
            
            
            glBindVertexArray(vao_particle);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // Don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint A5::firstUnusedParticle(){
    // Search from last used particle, this will usually return almost instantly
    for (GLuint i = last_particle; i < max_particles; ++i){
        if (particles[i].life <= 0.0f){
            last_particle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < last_particle; ++i){
        if (particles[i].life <= 0.0f){
            last_particle = i;
            return i;
        }
    }
    // Override first particle if all others are alive
    last_particle = 0;
    return 0;
}



/* --------------------------------------------------------------------------------------- */









