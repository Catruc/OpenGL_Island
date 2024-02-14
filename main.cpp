//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>

std::array<glm::vec3, 9> spotlightPositions = { glm::vec3(39.297f, 24.319f, -57.222f),
            glm::vec3(39.897f, 24.319f, -207.68f), glm::vec3(166.2f, 24.319f, -94.222f),
            glm::vec3(269.88f, 24.379f, -158.18f), glm::vec3(237.22f, 23.279f, -241.6f),
            glm::vec3(-16.283f, 23.279f, -121.34f), glm::vec3(81.377f, 23.279f, 80.718f),
            glm::vec3(-16.303f, 23.279f, 80.718f), glm::vec3(39.317f, 23.279f, 128.56f),
        };

std::array<glm::vec3, 2> spotLightPositionsAlternative = { glm::vec3(37.251f, 36.117f, 219.23f),
    glm::vec3(-236.94f, 31.014f, -121.53f)
};


// for point lights coordinates
glm::vec3 point1Position;
glm::vec3 color1Point;

glm::vec3 spot1Position;
glm::vec3 color2Spot;

float fov = 45.0f; // Initial FOV value, adjust as needed



bool rotateCamera = false;
float cameraRotationAngle = 0.0f; // in degrees




// for point light shader locations

GLint point1PositionLoc;
GLint color1PointLoc;

GLint spot1PositionLoc;
GLint color2SpotLoc;


float fogDensity = 0.0f;;
GLuint fogDensityLoc;



int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;
bool rotateBlades= false;

const unsigned int SHADOW_WIDTH = 2048*8;
const unsigned int SHADOW_HEIGHT = 2048*8;


float deltaTime = 0;
float lastFrame = 0.0f;
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 lightPosition;
GLuint lightPositionLoc;

glm::vec3 lightBulbColor;
GLuint lightBulbColorLoc;

GLuint pointOrNotLoc;



int pointOrNot = 0;
bool lightsOn = false;




gps::Camera myCamera(
    glm::vec3(300.0f, 600.0f,  -200.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f,  0.0f));
float cameraSpeed = 4.5f;

// pentru mouse la camera

float lastX = 400;
float lastY = 300;
bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;
bool updateCameraFromMouse = true;


//

bool pressedKeys[1024];
float angleY = 0.0f;
float angleE1 = 0.0f;
float angleE2 = 0.0f;
float eliceS = 50.0f;
GLfloat lightAngle;

gps::Model3D insula;
gps::Model3D screenQuad;
gps::Model3D nanosuit;
gps::Model3D elice1;
gps::Model3D elice2;

gps::Shader myCustomShader;
gps::Shader depthShader;
gps::Shader screenQuadShader;



GLuint shadowMapFBO;
GLuint depthMapTexture;




bool showDepthMap;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);

}





void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;
    
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        updateCameraFromMouse = !updateCameraFromMouse;
    }
    
    if(key == GLFW_KEY_4 && action == action == GLFW_PRESS)
    {
        fogDensity -= 0.0001;
        if(fogDensity<0)
        {
            fogDensity = 0;
        }
        glUniform1fv(fogDensityLoc, 1, &fogDensity);
        
    }
    
    
    if(key == GLFW_KEY_5 && action == action == GLFW_PRESS)
    {
        fogDensity += 0.0001;
        glUniform1fv(fogDensityLoc, 1, &fogDensity);
        
    }
    
    
    if(key == GLFW_KEY_6 && action == GLFW_PRESS) {
            rotateCamera = !rotateCamera; // Toggle camera rotation
        }
    
    
    
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        lightsOn = !lightsOn; // Toggle rotation
        if(lightsOn == true)
        {
            //0.8f, 0.8f, 0.0f
            color1Point = glm::vec3(0.8f, 0.8f, 0.0f);
            color1PointLoc = glGetUniformLocation(myCustomShader.shaderProgram, "color1");
            glUniform3fv(color1PointLoc, 1, glm::value_ptr(color1Point));
            
            glm::vec3 color2Spot = glm::vec3(0.8f, 0.5f, 0.0f); // Warm Red
            color2SpotLoc = glGetUniformLocation(myCustomShader.shaderProgram, "color2");
            glUniform3fv(color2SpotLoc, 1, glm::value_ptr(color2Spot));
            
        }
        else{
            color1Point = glm::vec3(0.0f, 0.0f, 0.0f);
            color1PointLoc = glGetUniformLocation(myCustomShader.shaderProgram, "color1");
            glUniform3fv(color1PointLoc, 1, glm::value_ptr(color1Point));
            
            glm::vec3 color2Spot = glm::vec3(0.0f, 0.0f, 0.0f);
            color2SpotLoc = glGetUniformLocation(myCustomShader.shaderProgram, "color2");
            glUniform3fv(color2SpotLoc, 1, glm::value_ptr(color2Spot));
        }
    }
    
    if(key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        lightDir = glm::vec3(0.0f, 0.0f, 0.0f);

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    
        lightColor = glm::vec3(0.25f, 0.25f, 0.55f) * 0.5f;
        lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        
        pointOrNot = 1;
        glUniform1i(pointOrNotLoc, pointOrNot);
    }
    
    if(key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        //set the light direction (direction towards the light)
        lightDir = glm::vec3(-23.82f, 321.89f, -19.54f);
        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //set light color
        lightColor = glm::vec3(1.0f, 0.9f, 0.7f); //white light
        lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        lightColor = glm::vec3(1.0f, 0.9f, 0.7f); //white light
        
        
        pointOrNot = 0;
        glUniform1i(pointOrNotLoc, pointOrNot);
    }
    
    
    if(action == GLFW_PRESS)
    {
        switch (key) {
            case GLFW_KEY_1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case GLFW_KEY_2:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case GLFW_KEY_3:
                glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
                break;
            
        }
    }

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}



void updateCameraRotation() {
    if (rotateCamera) {
        cameraRotationAngle += 1.0f;
        if (cameraRotationAngle > 360.0f) {
            cameraRotationAngle -= 360.0f;
        }

        float radius = 300.0f;
        float camX = sin(glm::radians(cameraRotationAngle)) * radius;
        float camZ = cos(glm::radians(cameraRotationAngle)) * radius;
        glm::vec3 cameraPosition = glm::vec3(camX, myCamera.getPosition().y, camZ);

        myCamera.setPosition(cameraPosition);

        
        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        myCamera.lookAtPoint(center);

        
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }
}




void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!updateCameraFromMouse) {
        return;
    }
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::max(-89.0f, std::min(pitch, 89.0f)); // Clamp pitch

    // Update the camera's orientation
    myCamera.rotate(pitch, yaw);

    view = myCamera.getViewMatrix();
    myCustomShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void processMovement()
{
    float rotationSpeed = 2.0f;
    
    
    
	if (pressedKeys[GLFW_KEY_R]) {
        myCamera.rotate(pitch, yaw-=1.0f);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_T]) {
        myCamera.rotate(pitch, yaw+=1.0f);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}
    
    if (pressedKeys[GLFW_KEY_U]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    
    if (pressedKeys[GLFW_KEY_P]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    
    
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetScrollCallback(glWindow, scroll_callback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);
    
    
	glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);
    
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	insula.LoadModel("objects/obiect1/test.obj");
    screenQuad.LoadModel("objects/Quad/quad.obj");
    elice1.LoadModel("objects/Elice1/elice1.obj");
    elice2.LoadModel("objects/Elice2/elice2.obj");
	
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthShader.loadShader("shaders/depth.vert", "shaders/depth.frag");
    depthShader.useShaderProgram();

	//myCustomShader.useShaderProgram();
	
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
    projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(-23.82f, 321.89f, -19.54f);
    //-23.82f, 321.89f, -19.54f
    //lightDir = glm::vec3(-23.82f, 321.89f, -19.54f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 0.9f, 0.7f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    
    //set light color
    lightBulbColor = glm::vec3(255, 0, 0); //white light
    lightBulbColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColorBulb");
    glUniform3fv(lightBulbColorLoc, 1, glm::value_ptr(lightColor));
    
    //set the spotOrNotLocation
    pointOrNotLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointOrNot");
    
    
    // init point lights
    
    
    for(int i = 0; i<spotlightPositions.size()  ;i++)
    {
        glm::vec3 point1Position = spotlightPositions[i];
        point1PositionLoc = glGetUniformLocation(myCustomShader.shaderProgram, ("light" + std::to_string(i + 1)).c_str());
        glUniform3fv(point1PositionLoc, 1, glm::value_ptr(point1Position));
            
        color1Point = glm::vec3(0.0f, 0.0f, 0.0f);
        color1PointLoc = glGetUniformLocation(myCustomShader.shaderProgram, "color1");
        glUniform3fv(color1PointLoc, 1, glm::value_ptr(color1Point));
            
    }
    
    
    for(int i = 0; i<spotLightPositionsAlternative.size()  ;i++)
    {
        glm::vec3 spot1Position = spotLightPositionsAlternative[i];
        spot1PositionLoc = glGetUniformLocation(myCustomShader.shaderProgram, ("spotlight" + std::to_string(i + 1)).c_str());
        glUniform3fv(spot1PositionLoc, 1, glm::value_ptr(spot1Position));
            
        glm::vec3 color2Spot = glm::vec3(0.0f, 0.0f, 0.0f);

        color2SpotLoc = glGetUniformLocation(myCustomShader.shaderProgram, "color2");
        glUniform3fv(color2SpotLoc, 1, glm::value_ptr(color2Spot));
            
    }
        

    fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
    glUniform1fv(fogDensityLoc, 1, &fogDensity);

    
    //point1Position = glm::vec3(39.297f, 24.319f, -57.222f);
    //point1PositionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "light1");
    //glUniform3fv(point1PositionLoc, 1, glm::value_ptr(point1Position));
    
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
        
    glGenFramebuffers(1, &shadowMapFBO);

        //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
        //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 400.5f;
    glm::mat4 lightProjection = glm::ortho(-572.072f, 572.072f, -572.072f, 572.072f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;


    return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	insula.Draw(shader);
    
    
}


void drawingObjectE1(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();
    
    
    glm::mat4 modelElice1 = model *
        glm::translate(glm::mat4(1.0f), glm::vec3(168.2f, 64.97f, 159.5f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(angleE1), glm::vec3(1.0f, 0.0f, 1.0f)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(-168.2f, -64.97f, -159.5f));

    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelElice1));
    
    elice1.Draw(shader);
}


void drawingObjectE2(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();
    
    
    glm::mat4 modelElice2 = model *
    glm::translate(glm::mat4(1.0f), glm::vec3(-221.1f, 64.97f, -15.41f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(angleE2), glm::vec3(1.0f, 0.0f, 1.0f)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(+221.1f, -64.97f, +15.41f));

    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelElice2));
    
    elice2.Draw(shader);
}





void moveObjects()
{
    angleE1 += eliceS * deltaTime;
    angleE2 += eliceS * deltaTime;
}



void renderScene() {

    // depth maps creation pass
    //TODO - Send the light-space transformation matrix to the depth map creation shader and
    //         render the scene in the depth map

    depthShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"),1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix()));
    
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    drawObjects(depthShader, true);
    drawingObjectE1(depthShader,true);
    drawingObjectE2(depthShader,true);
    
    //renderScene();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    // render depth map on screen - toggled with the M key

    if (showDepthMap) {
        
        ///////umbre
        
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
        
        
        
        
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myCustomShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                
        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        drawObjects(myCustomShader, false);

        
        drawingObjectE1(myCustomShader, true);
        drawingObjectE2(myCustomShader, true);
        moveObjects();
        
    }
}



void updateProjection(float fov) {
    projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}



void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
    
    
    
    

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
        
    float frame = glfwGetTime();
    deltaTime = frame - lastFrame;
    lastFrame = frame;
        
    processMovement();
    updateCameraRotation();
    renderScene();

    glfwPollEvents();
    glfwSwapBuffers(glWindow);
    }

	cleanup();

	return 0;
}
