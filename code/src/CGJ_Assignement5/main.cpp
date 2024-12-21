////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
// Copyright (c) 2023-24 by Carlos Martinho
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "../mgl/mgl.hpp"

#include "InputManager.hpp"
#include "SceneNode.hpp"

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
    void initCallback(GLFWwindow* win) override;
    void displayCallback(GLFWwindow* win, double elapsed) override;
    void windowSizeCallback(GLFWwindow* win, int width, int height) override;
    void mouseButtonCallback(GLFWwindow* window, int button, int action,
        int mods) override;
    void cursorCallback(GLFWwindow* window, double xpos, double ypos) override;
    //void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;

private:
    const GLuint UBO_BP = 0;
    mgl::ShaderProgram* Shaders = nullptr;
    mgl::Camera* Camera = nullptr;
    InputManager *inputManager = nullptr;
    SceneNode* root = nullptr;
    GLint ModelMatrixId;
    mgl::Mesh* Mesh = nullptr;

    mgl::Mesh* createMesh(std::string meshFile);
    void createShaderPrograms(std::string shaderName);
    void createSceneGraph();
    void createCamera();
    void createInputManager();
    void drawScene();
};

///////////////////////////////////////////////////////////////////////// MESHES

mgl::Mesh* MyApp::createMesh(std::string meshFile) {
    std::string mesh_dir = "../assets/";
    std::string mesh_fullname = mesh_dir + meshFile;


    mgl::Mesh* mesh = new mgl::Mesh();
    mesh->joinIdenticalVertices();
    mesh->create(mesh_fullname);
    return mesh;
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms(std::string shaderName) {
    Shaders = new mgl::ShaderProgram();

    std::string vertexShader = "../shaders/" + shaderName + ".vert";
    std::string fragmentShader = "../shaders/" + shaderName + ".frag";

    Shaders->addShader(GL_VERTEX_SHADER, vertexShader.c_str());
    Shaders->addShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());

    Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
    Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);

    Shaders->addUniform(mgl::MODEL_MATRIX);
    Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
    Shaders->create();

    ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    root = new SceneNode(nullptr, Shaders);

    //////add children
    //wooden base
    mgl::Mesh* woodMesh = createMesh("wooden-base/bunny-vn-smooth.obj");
    root->addChild(new SceneNode(woodMesh, Shaders));

    //get the children vector
    std::vector<SceneNode*> children = root->getChildren();
    
    //set the wooden base a bit lower
    //children[0]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    children[0]->createTexturePerlin(512, 512);
    //children[0]->createTextureImage("wooden-base/wooden-base.jpg");
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::createCamera() {
    Camera = new mgl::Camera(UBO_BP);
}

///////////////////////////////////////////////////////////////////////// INPUT MANAGER

void MyApp::createInputManager() {
    inputManager = new InputManager;
    inputManager->setCamera(Camera);
}

///////////////////////////////////////////////////////////////////////// DRAW

void MyApp::drawScene() {
    root->draw(ModelMatrixId, glGetUniformLocation(Shaders->ProgramId, "viewPos"), Camera->getEye());
}

///////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
    createShaderPrograms("toon");
    createSceneGraph();
    createCamera();
    createInputManager();
}

void MyApp::windowSizeCallback(GLFWwindow* win, int width, int height) {
    inputManager->windowSizeCallback(win, width, height);
}

void MyApp::displayCallback(GLFWwindow* win, double elapsed) { drawScene(); }

void MyApp::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    inputManager->mouseButtonCallback(window, button, action, mods);
}

void MyApp::cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    inputManager->cursorCallback(window, xpos, ypos);
}

void MyApp::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    inputManager->scrollCallback(window, xoffset, yoffset);
}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
    mgl::Engine& engine = mgl::Engine::getInstance();
    engine.setApp(new MyApp());
    engine.setOpenGL(4, 6);
    engine.setWindow(800, 600, "Mesh Loader", 0, 1);
    engine.init();
    engine.run();
    exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
