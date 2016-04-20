#ifndef QUICKVERTEXBUFFER_H
#define QUICKVERTEXBUFFER_H

#include <GL/glextl.h>
#include <vector>
#include <glm/glm.hpp>

class QuickVertexBuffer
{
public:
    QuickVertexBuffer(GLuint shapeType, const std::vector<glm::vec3>& vertices);
    virtual ~QuickVertexBuffer();

    void Render(const glm::mat4& matrix);

    GLuint _shapeType;
    GLuint _vertexCount;
    GLuint _program;
    GLuint _u_matrix;
    GLuint _vertexArrayObject;
    GLuint _vertexBufferObject;

};

#endif // QUICKVERTEXBUFFER_H
