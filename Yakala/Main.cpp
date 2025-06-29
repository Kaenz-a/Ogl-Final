#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <iostream>

#include "ogldev_engine_common.h"
#include "ogldev_util.h"
#include "ogldev_basic_glfw_camera.h"
#include "ogldev_new_lighting.h"
#include "ogldev_glfw.h"
#include "ogldev_basic_mesh.h"
#include "ogldev_world_transform.h"
#include "ogldev_phong_renderer.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void CursorPosCallback(GLFWwindow* window, double x, double y);
static void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode);
static SkinnedMesh* m_pMesh = NULL;

class Tutorial40
{
public:

    Tutorial40()
    {
        m_dirLight.WorldDirection = Vector3f(0.0f, -1.0f, 1.0f);
        m_dirLight.DiffuseIntensity = 1.0f;
        m_dirLight.AmbientIntensity = 0.5f;
    }

    virtual ~Tutorial40()
    {
        SAFE_DELETE(m_pGameCamera);
    }


    void Init()
    {
        CreateWindow();

        InitCallbacks();

        InitCamera();

        InitMesh();

        InitRenderer();

        m_startTime = GetCurrentTimeMillis();
        m_currentTime = m_startTime;
    }


    void Run()
    {
        while (!glfwWindowShouldClose(window)) {
            RenderSceneCB();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }


    void RenderSceneCB()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pGameCamera->OnRender();

        if (m_runAnimation) {
            m_currentTime = GetCurrentTimeMillis();
        }

        float AnimationTimeSec = (float)((double)m_currentTime - (double)m_startTime) / 1000.0f;

        float TotalPauseTimeSec = (float)((double)m_totalPauseTime / 1000.0f);
        AnimationTimeSec -= TotalPauseTimeSec;
        m_phongRenderer.RenderAnimation(m_pMesh, AnimationTimeSec, m_animationIndex);
        m_phongRenderer.Render(m_pTerrain);
        m_phongRenderer.Render(m_pTerrain2);
        m_phongRenderer.Render(m_pTerrain3);

    }


#define ATTEN_STEP 0.01f

#define ANGLE_STEP 1.0f

    void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

    void KeyboardCB(uint key, int state)
    {        
        if (state == GLFW_PRESS) {

            switch (key) {

            case GLFW_KEY_SPACE:
                m_runAnimation = !m_runAnimation;
                if (m_runAnimation) {
                    long long CurrentTime = GetCurrentTimeMillis();
                    // printf("Resumed at %lld\n", CurrentTime);
                    m_totalPauseTime += (CurrentTime - m_pauseStart);
                    // printf("Total pause time %lld\n", m_totalPauseTime);
                } else {
                    m_pauseStart = GetCurrentTimeMillis();
                    // printf("Paused at %lld\n", GetCurrentTimeMillis());
                }
                break;

            case GLFW_KEY_ESCAPE:
                glfwDestroyWindow(window);
                glfwTerminate();
                exit(0);
            }
        }

        m_pGameCamera->OnKeyboard(key);
    }


    void MouseCB(int button, int action, int x, int y)
    {
    }


public:

    void CreateWindow()
    {
        int major_ver = 0;
        int minor_ver = 0;
        bool is_full_screen = false;
        window = glfw_init(major_ver, minor_ver, WINDOW_WIDTH, WINDOW_HEIGHT, is_full_screen, "Tutorial 40");

        glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    }


    void InitCallbacks()
    {
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
    }


    void InitCamera()
    {
        Vector3f Pos(0.0f, 10.0f, -50.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        float FOV = 90.0f;
        float zNear = 0.1f;
        float zFar = 1000.0f;
        PersProjInfo persProjInfo = { FOV, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, zNear, zFar };

        m_pGameCamera = new BasicCamera(persProjInfo, Pos, Target, Up);
    }


    void InitRenderer()
    {
        m_phongRenderer.InitPhongRenderer();
        m_phongRenderer.SetCamera(m_pGameCamera);
        m_phongRenderer.SetDirLight(m_dirLight);
    }


    void InitMesh()
    {
        m_pMesh = new SkinnedMesh();
        
        if (!m_pMesh->LoadMesh("../Content/Snatch.dae")) {
            printf("Missing mesh file\n");
            printf("You can download it from %s\n", 
                "https://sketchfab.com/3d-models/iclone-7-raptoid-mascot-free-download-56a3e10a73924843949ae7a9800c97c7");
        }


       // m_pMesh->LoadMesh("../Content/boblampclean.md5mesh");
        m_pMesh->SetRotation(0.0f, 180.0f, 0.0f);
        m_pMesh->SetPosition(2.0f, -1000.0f, 17.5f);
        m_pMesh->SetScale(0.1f);


        m_pTerrain = new BasicMesh();
        if (!m_pTerrain->LoadMesh("../Content/terrain_rock_boulder_cracked.obj")) {
            printf("Error loading mesh ../Content/terrain_rock_boulder_cracked.obj");
            exit(0);
        }
        m_pTerrain->SetRotation(0.0f, 0.0f, 0.0f);
        m_pTerrain->SetPosition(0.0f, 0.0f, 0.0f);
        m_pTerrain->SetScale(10.0f);

        m_pTerrain2 = new BasicMesh();
        if (!m_pTerrain2->LoadMesh("../Content/portalbutton.obj")) {
            printf("Error loading mesh ../Content/portalbutton.obj");
            exit(0);
        }
        m_pTerrain2->SetRotation(0.0f, 0.0f, 0.0f);
        m_pTerrain2->SetPosition(-20.0f, 0.0f, 20.0f);
        m_pTerrain2->SetScale(0.1f);

        m_pTerrain3 = new BasicMesh();
        if (!m_pTerrain3->LoadMesh("../Content/portalbutton.obj")) {
            printf("Error loading mesh ../Content/portalbutton.obj");
            exit(0);
        }
        m_pTerrain3->SetRotation(0.0f, 0.0f, 0.0f);
        m_pTerrain3->SetPosition(20.0f, 0.0f, 20.0f);
        m_pTerrain3->SetScale(0.1f);
    }

    GLFWwindow* window = NULL;
    BasicCamera* m_pGameCamera = NULL;
    PhongRenderer m_phongRenderer;
    
    BasicMesh* m_pTerrain3 = NULL;
    BasicMesh* m_pTerrain2 = NULL;
    BasicMesh* m_pTerrain = NULL;
    PersProjInfo m_persProjInfo;
    DirectionalLight m_dirLight;
    long long m_startTime = 0;
    long long m_currentTime = 0;
    bool m_runAnimation = true;
    long long m_totalPauseTime = 0;
    long long m_pauseStart = 0;
    int m_animationIndex = 0;
};

Tutorial40* app = NULL;

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app->KeyboardCB(key, action);
}


static void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    app->PassiveMouseCB((int)x, (int)y);
}


static void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode)
{
    double x, y;

    glfwGetCursorPos(window, &x, &y);

    app->MouseCB(Button, Action, (int)x, (int)y);
}


int main(int argc, char** argv)
{
    app = new Tutorial40();

    app->Init();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    app->Run();

    delete app;

    return 0;
}
