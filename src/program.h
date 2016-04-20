#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <GL/glextl.h>
#include <glm/glm.hpp>
#include "common/application.h"
#include "common/camera.h"
#include <hl1types.h>

class Program : public Application
{
    int _width, _height;

public:
    Program();
    virtual ~Program();

    int Width() { return this->_width; }
    int Height() { return this->_height; }
public:
    virtual const char* GetWindowTitle() { return "cs-tribute"; }
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

    Hl1Asset* _asset;
    Hl1Instance* _instance;

};


#endif // _PROGRAM_H_
