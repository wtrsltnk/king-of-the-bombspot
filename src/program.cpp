#include "program.h"
#include "filesystem.h"
#include "common/settings.h"
#include "common/log.h"
#include "quickvertexbuffer.h"

#include <valve/hl1bspinstance.h>
#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

using namespace valve;

Application* gApp = new Program();
static FileLoggingStrategy fileLogging;

glm::mat4 Object::LocalMatrix() const
{
    return glm::translate(glm::toMat4(this->_rotation), this->_position);
}

glm::mat4 Object::WorldMatrix() const
{
    glm::mat4 view = this->LocalMatrix();

    if (this->_parent != nullptr) view = this->_parent->WorldMatrix() * this->LocalMatrix();

    return view;
}

void Object::Render(const glm::mat4 &proj)
{
    if (this->_instance != nullptr)
    {
        this->_instance->Render(proj, this->WorldMatrix());
    }
}

Program::Program()
    : _pan(false), _lastX(0), _lastY(0), _asset(nullptr), _instance(nullptr)
{
    Settings::Instance()->LoadFromDisk("king-of-the-bombspot.settings");
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

typedef struct {
    int index;
    std::set<int> neighbours;
    std::vector<int> cornerIndices;

} element;

QuickVertexBuffer* buf = nullptr;
Array<element> edges;
Array<element> faces;

std::ostream& operator << (std::ostream& os, const glm::vec2& v)
{
    os << "[" << v[0] << ", " << v[1] << "]";
    return os;
}

std::ostream& operator << (std::ostream& os, const glm::vec3& v)
{
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
    return os;
}

std::ostream& operator << (std::ostream& os, const glm::vec4& v)
{
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]";
    return os;
}

glm::vec3 ParseVec3(const std::string& str)
{
    double x, y, z;
    std::stringstream ss(str);

    ss >> x >> y >> z;

    return glm::vec3(x, y, z);
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
        this->_asset = new hl1::BspAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
        if (this->_asset != nullptr && this->_asset->Load(filename))
        {
            this->_instance = new hl1::BspInstance(this->_asset);

            auto mdlAsset = new hl1::MdlAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
            mdlAsset->Load("..\\king-of-the-bombspot\\data\\sas.mdl");

            for (auto itr = this->_asset->_entities.begin(); itr != _asset->_entities.end(); ++itr)
            {
                hl1::tBSPEntity& entity = *itr;
                if (entity.classname == "info_player_start"
                        && entity.keyvalues.find("origin") != entity.keyvalues.end())
                {
                    std::string origin = entity.keyvalues.at("origin");

                    auto obj = new Object();
                    obj->_instance = new hl1::MdlInstance(mdlAsset);
                    ((hl1::MdlInstance*)obj->_instance)->SetSequence(9, true);
                    obj->_position = ParseVec3(origin);
                    this->_objects.push_back(obj);
                }
            }

            edges.Allocate(this->_asset->_edgeData.count);
            for (int f = 0; f < this->_asset->_faceData.count; f++)
            {
                hl1::tBSPFace& face = this->_asset->_faceData[f];
                for (int e = 0; e < face.edgeCount; e++)
                {
                    int ei = this->_asset->_surfedgeData[face.firstEdge + e];
                    edges[ei < 0 ? -ei : ei].index = ei < 0 ? -ei : ei;
                    edges[ei < 0 ? -ei : ei].neighbours.insert(f);
                    edges[ei < 0 ? -ei : ei].cornerIndices.push_back(_asset->_edgeData[ei < 0 ? -ei : ei].vertex[0]);
                    edges[ei < 0 ? -ei : ei].cornerIndices.push_back(_asset->_edgeData[ei < 0 ? -ei : ei].vertex[1]);
                }
            }

            faces.Allocate(this->_asset->_faceData.count);
            for (int f = 0; f < this->_asset->_faceData.count; f++)
            {
                hl1::tBSPFace& face = this->_asset->_faceData[f];
                for (int e = 0; e < face.edgeCount; e++)
                {
                    int ei = this->_asset->_surfedgeData[face.firstEdge + e];
                    auto edge = edges[ei < 0 ? -ei : ei];
                    faces[f].index = f;
                    faces[f].neighbours.insert(edge.neighbours.begin(), edge.neighbours.end());
                    faces[f].cornerIndices.push_back(_asset->_edgeData[ei < 0 ? -ei : ei].vertex[0]);
                    faces[f].cornerIndices.push_back(_asset->_edgeData[ei < 0 ? -ei : ei].vertex[1]);
                }
                faces[f].neighbours.erase(f);
            }

            std::vector<glm::vec3> verts;
            for (int v = 0; v < _asset->_verticesData.count; v++) verts.push_back(_asset->_verticesData[v].point);
            buf = new QuickVertexBuffer(GL_LINES, verts);
        }
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
        if (this->_instance != nullptr)
            this->_instance->Update(updateDiff);

        for (auto obj = this->_objects.begin(); obj != this->_objects.end(); ++obj)
            (*obj)->_instance->Update(updateDiff);

        lastUpdateTime = time;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_ALPHA_TEST);
//    for (int i = 0; i < edges.count; i++)
//    {
//        buf->RenderSubSet(this->_proj * this->_cam.GetViewMatrix(), edges[i].cornerIndices);
//    }
//    for (int i = 0; i < faces.count; i++)
//    {
//        if (glm::dot(_asset->_va.Faces()[i].plane.normal, glm::vec3(0.0f, 0.0f, 1.0f)) < 0.7f)
//            continue;
//        if (_asset->FaceFlags(i) != 0)
//            continue;
//        if (_asset->_textures[_asset->_faces[i].texture].Name()[0] == '!')
//            continue;
//        buf->RenderSubSet(this->_proj * this->_cam.GetViewMatrix(), faces[i].cornerIndices);
//    }

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.8f);
    if (this->_instance != nullptr)
    {
        this->_instance->Render(this->_proj, this->_cam.GetViewMatrix());
    }

    for (auto obj = this->_objects.begin(); obj != this->_objects.end(); ++obj)
        (*obj)->Render(this->_proj * this->_cam.GetViewMatrix());
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
