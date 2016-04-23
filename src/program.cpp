#include "program.h"
#include "filesystem.h"
#include "common/settings.h"
#include "common/log.h"

#include <hl1bspinstance.h>
#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Application* gApp = new Program();
static FileLoggingStrategy fileLogging;

Program::Program()
    : _pan(false), _lastX(0), _lastY(0), _asset(nullptr), _instance(nullptr)
{
    Settings::Instance()->LoadFromDisk("king-of-the-bombspot.settings");
    Setting("Viewer.PauseAnimation").Register(false);
    Setting("Viewer.Camera.Speed").Register(200.0f);
    Logging::Instance()->SetStrategy(&fileLogging);
}

Program::~Program()
{ }

bool Program::InitializeApplication(System* sys)
{
    this->_sys = sys;

    return true;
}

bool Program::InitializeGraphics()
{
    std::cout << "GL_VERSION                  : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GL_RENDERER                 : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL_VENDOR                   : " << glGetString(GL_VENDOR) << std::endl;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    this->_cam.MoveForward(-120.0f);

    if (this->_sys->GetArgs().size() > 1)
    {
        std::string filename = this->_sys->GetArgs()[1];

        this->_asset = new Hl1BspAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
        if (this->_asset != nullptr
                && this->_asset->Load(filename))
            this->_instance = this->_asset->CreateInstance();
    }
    return true;
}

void Program::GameLoop()
{
    static double lastTime = this->_sys->GetTime();
    static double lastUpdateTime = this->_sys->GetTime();

    double time = this->_sys->GetTime();
    double diff = time - lastTime;
    double speed = double(Setting("Viewer.Camera.Speed").AsFloat());

    if (this->_sys->IsKeyDown(KeyCodes::Character_A)) this->_cam.MoveLeft(diff * speed);
    else if (this->_sys->IsKeyDown(KeyCodes::Character_D)) this->_cam.MoveLeft(-diff * speed);

    if (this->_sys->IsKeyDown(KeyCodes::Character_W)) this->_cam.MoveForward(diff * speed);
    else if (this->_sys->IsKeyDown(KeyCodes::Character_S)) this->_cam.MoveForward(-diff * speed);

    lastTime = time;

    double updateDiff = time - lastUpdateTime;
    if (updateDiff > 1.0/60.0)
    {
        if (this->_instance != nullptr && Setting("Viewer.PauseAnimation").AsBool() == false)
            this->_instance->Update(updateDiff);

        lastUpdateTime = time;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.8f);

    if (this->_instance != nullptr)
        this->_instance->Render(this->_proj, this->_cam.GetViewMatrix());
}

bool Program::IsRunning()
{
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    return !state[SDL_SCANCODE_ESCAPE];
}

void Program:: Resize(int w, int h)
{
    if (h < 1) h = 1;

    glViewport(0, 0, w, h);

    this->_proj = glm::perspective(120.0f, float(w) / float(h), 0.1f, 4000.0f);
}

void Program::MouseButtonDown(int button, int x, int y)
{
    this->_pan = (button == SDL_BUTTON_LEFT);
    this->_lastX = x;
    this->_lastY = y;
}

void Program::MouseMove(int x, int y)
{
    if (this->_pan)
    {
        this->_cam.RotateZ(glm::radians(float(this->_lastX-x) * 0.1f));
        this->_cam.RotateX(glm::radians(float(this->_lastY-y) * 0.1f));
    }

    this->_lastX = x;
    this->_lastY = y;
}

void Program::MouseButtonUp(int button, int x, int y)
{
    this->_pan = false;
}

void Program::MouseWheel(int x, int y)
{ }

void Program::KeyAction(int key, int action)
{
    if (key == SDLK_SPACE && action) Setting("Viewer.PauseAnimation") = !Setting("Viewer.PauseAnimation").AsBool();
    else if (key == SDLK_KP_PLUS && action) Setting("Viewer.Camera.Speed") = Setting("Viewer.Camera.Speed").AsFloat() + 5.0f;
    else if (key == SDLK_KP_MINUS && action) Setting("Viewer.Camera.Speed") = Setting("Viewer.Camera.Speed").AsFloat() - 5.0f;
}

void Program::Close()
{
    this->_running = false;
}

void Program::Destroy()
{
    Settings::Instance()->SaveToDisk("king-of-the-bombspot.settings");
}

int Program::GetWindowFlags()
{
    return SDL_WINDOW_BORDERLESS;
}
