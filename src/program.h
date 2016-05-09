#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "common/application.h"
#include "common/camera.h"
#include <hl1bspasset.h>
#include <hl1bspinstance.h>
#include <hl1mdlinstance.h>

class Object
{
public:
    Object() : _parent(nullptr), _instance(nullptr) { }

    glm::mat4 LocalMatrix() const;
    glm::mat4 WorldMatrix() const;

    Object* _parent;
    glm::quat _rotation;
    glm::vec3 _position;
    Hl1Instance* _instance;

    void Render(const glm::mat4& proj);
};

class Program : public Application
{
    int _width, _height;

public:
    Program();
    virtual ~Program();

    int Width() { return this->_width; }
    int Height() { return this->_height; }
public:
    virtual const char* GetWindowTitle() { return "King of the Bombspot"; }
    virtual void GetContextAttributes(int& major, int& minor, bool& core) { major = 3; minor = 1; core = false; }
    virtual int GetWindowFlags();

    virtual bool InitializeApplication(System* sys);
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
    bool _running;
    System* _sys;
    glm::mat4 _proj;
    int _lastX, _lastY;
    bool _pan;
    Camera _cam;

    Hl1BspAsset* _asset;
    Hl1BspInstance* _instance;

    std::vector<Object*> _objects;

};


#endif // _PROGRAM_H_
