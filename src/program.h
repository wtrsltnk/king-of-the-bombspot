#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "common/application.h"
#include "common/camera.h"
#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <valve/hl1bspasset.h>
#include <valve/hl1bspinstance.h>
#include <valve/hl1mdlinstance.h>

using namespace valve;

class Object
{
public:
    Object() : _parent(nullptr), _instance(nullptr) {}

    glm::mat4 LocalMatrix() const;
    glm::mat4 WorldMatrix() const;

    Object *_parent;
    glm::quat _rotation;
    glm::vec3 _position;
    AssetInstance *_instance;

    void Render(const glm::mat4 &proj);
};

class Program : public Application
{
public:
    Program();
    virtual ~Program();

    int Width() { return this->_width; }
    int Height() { return this->_height; }

public:
    virtual const char *GetWindowTitle() { return "King of the Bombspot"; }
    virtual void GetContextAttributes(int &major, int &minor, bool &core)
    {
        major = 3;
        minor = 1;
        core = false;
    }
    virtual int GetWindowFlags();

    virtual bool InitializeApplication(System *sys);
    virtual bool InitializeGraphics();
    virtual void GameLoop();
    virtual bool IsRunning();
    void StopRunning();
    virtual void Resize(int w, int h);
    virtual void MouseMove(int x, int y);
    virtual void MouseButtonDown(int exit, int x, int y);
    virtual void MouseButtonUp(int exit, int x, int y);
    virtual void MouseWheel(int x, int y);
    virtual void KeyAction(int key, int action);
    virtual void Close();
    virtual void Destroy();

private:
    int _width = 1024;
    int _height = 768;
    bool _running = true;
    System *_sys = nullptr;
    glm::mat4 _proj;
    int _lastX = 0;
    int _lastY = 0;
    bool _pan = false;
    Camera _cam;

    hl1::BspAsset *_asset = nullptr;
    hl1::BspInstance *_instance = nullptr;

    std::vector<Object *> _objects;
};

#endif // _PROGRAM_H_
