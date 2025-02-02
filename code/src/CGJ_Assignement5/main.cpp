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
    mgl::Camera* Camera = nullptr;
    InputManager *inputManager = nullptr;
    SceneNode* root = nullptr;
    mgl::Mesh* Mesh = nullptr;

    mgl::Mesh* createMesh(std::string meshFile);
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

///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    mgl::PhongShader* phongShader = new mgl::PhongShader();
    mgl::ToonShader* toonShader = new mgl::ToonShader();
    mgl::GlassShader* glassShader = new mgl::GlassShader();
    mgl::OutlineShader* outlineShader = new mgl::OutlineShader();
    mgl::SkyboxShader* skyboxShader = new mgl::SkyboxShader();

    root = new SceneNode(nullptr, nullptr);

    //////step 1: create children with desired shaders
    //1 - skybox
    skyboxShader->createShaderPrograms("skybox", "skybox");
    mgl::Mesh* skyMesh = createMesh("skybox/cube-vt4.obj");
    root->addChild(new SceneNode(skyMesh, skyboxShader));

    //2 - base
    phongShader->createShaderPrograms("default", "default");
    mgl::Mesh* baseMesh = createMesh("base/base.obj");
    root->addChild(new SceneNode(baseMesh, phongShader));

    //3 - display object
    outlineShader->createShaderPrograms("outline", "outline");
    mgl::Mesh* objectMesh = createMesh("display-object/Marcus_Arelius.obj");
    root->addChild(new SceneNode(objectMesh, outlineShader));
    toonShader->createShaderPrograms("toon", "toon");
    root->getChildren().at(2)->addChild(new SceneNode(objectMesh, toonShader));

    //4 - glass dome
    glassShader->createShaderPrograms("glass", "glass");
    mgl::Mesh* domeMesh = createMesh("dome/dome.obj");
    root->addChild(new SceneNode(domeMesh, glassShader));

    //////step 2: set each nodes characteristics
    ////get the children vector
    std::vector<SceneNode*> children = root->getChildren();
    
    //create some variables
    glm::vec3 lightPos(-0.5f, 2.0f, 0.5f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    int ToonColorSteps = 3;

    ////1 - skybox
    children[0]->createTextureSkybox("skybox/sky", "jpg");
    children[0]->setSkybox(true);

    ////2 - base (phong shading)
    children[1]->createTexturePerlin(512, 512);
    children[1]->addVec3Uniform("lightPos", lightPos);
    children[1]->addVec3Uniform("lightColor", lightColor);
    children[1]->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));

    ////3 - display object (toon/outline)
    //outline (outline)
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    children[2]->setScale(glm::vec3(0.00005, 0.00005, 0.00005)); //why is it so big jesus
    children[2]->setPosition(glm::vec3(0.6f, 0.0f, 0.6f));
    children[2]->setRotation(glm::vec3(90.0f, 0.0f, 0.0f));
    children[2]->setOutline(true);
    children[2]->addFloatUniform("OutlineThickness", 0.5);
    //object (toon)
    //children[2]->getChildren().at(0)->createTexturePerlin(512, 512);
    children[2]->getChildren().at(0)->createTextureColor(512, 512, 255, 255, 255);
    children[2]->getChildren().at(0)->addIntUniform("colorSteps", ToonColorSteps);
    children[2]->getChildren().at(0)->addVec3Uniform("lightPos", lightPos);
    children[2]->getChildren().at(0)->addVec3Uniform("lightColor", lightColor);

    ////4 = glass dome
    children[3]->createTextureSkybox("skybox/sky", "jpg");
    children[3]->addFloatUniform("refractiveIndex", 0.96f);
    children[3]->addFloatUniform("airIndex", 1.0f);
    children[3]->addFloatUniform("alpha", 0.3f);
    children[3]->setPosition(glm::vec3(0.0f, 0.25f, 0.0f));
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
