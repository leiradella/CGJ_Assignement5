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
    mgl::Mesh* Mesh = nullptr;

    mgl::Mesh* createMesh(std::string meshFile);
    void createShaderPrograms(std::string vertexName, std::string fragmentName);
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

void MyApp::createShaderPrograms(std::string vertexName, std::string fragmentName) {
    Shaders = new mgl::ShaderProgram();

    std::string vertexShader = "../shaders/" + vertexName + ".vert";
    std::string fragmentShader = "../shaders/" + fragmentName + ".frag";

    Shaders->addShader(GL_VERTEX_SHADER, vertexShader.c_str());
    Shaders->addShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());

    //Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
    //Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
    //Shaders->addUniform(mgl::MODEL_MATRIX);
    //Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
    
    Shaders->create();
}

///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    root = new SceneNode(nullptr, Shaders);

    //////step 1: create children with desired shaders
    //1 - wooden base
    createShaderPrograms("default", "default");
    mgl::Mesh* baseMesh = createMesh("wooden-base/wooden-base.obj");
    root->addChild(new SceneNode(baseMesh, Shaders));

    //2 - display object
    createShaderPrograms("outline", "outline");
    mgl::Mesh* objectMesh = createMesh("wooden-base/wooden-base.obj");
    root->addChild(new SceneNode(objectMesh, Shaders));
    createShaderPrograms("default", "toon");
    root->getChildren().at(1)->addChild(new SceneNode(objectMesh, Shaders));
    
    //3 - skybox
    createShaderPrograms("skybox", "skybox");
    mgl::Mesh* skyMesh = createMesh("skybox/cube-vt4.obj");
    root->addChild(new SceneNode(skyMesh, Shaders));

    //////step 2: set each nodes characteristics
    ////get the children vector
    std::vector<SceneNode*> children = root->getChildren();
    
    ////1 - wooden base (phong shading)
    children[0]->createTexturePerlin(512, 512);
    children[0]->addVec3Uniform("lightPos", glm::vec3(0.0f, 2.0f, 0.0f));
    children[0]->addVec3Uniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    children[0]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    ////2 - display object (toon/outline)
    //outline (outline)
    children[1]->setScale(glm::vec3(1.05f, 1.05f, 1.05f));
    children[1]->setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    children[1]->setOutline(true);
    //object (toon)
    children[1]->getChildren().at(0)->createTextureColor(512, 512, 122, 27, 186);
    children[1]->getChildren().at(0)->setScale(glm::vec3(1/1.05, 1/1.05, 1/1.05));
    children[1]->getChildren().at(0)->addIntUniform("colorSteps", 3);

    ////3 - skybox
    //children[2]->createTextureImage("skybox/sky-right.jpg");
    //children[2]->addVec3Uniform("lightPos", glm::vec3(0.0f, 2.0f, 0.0f));
    //children[2]->addVec3Uniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    children[2]->createTextureSkybox("skybox/sky", "jpg");
    children[2]->setSkybox(true);
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
    root->draw();
}

///////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
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
