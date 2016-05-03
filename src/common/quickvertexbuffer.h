#ifndef QUICKVERTEXBUFFER_H
#define QUICKVERTEXBUFFER_H

#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <vector>

class QuickVertexBuffer
{
public:
    QuickVertexBuffer(GLuint shapeType, const std::vector<glm::vec3>& vertices);
    virtual ~QuickVertexBuffer();

    void Render(const glm::mat4& matrix);
    void RenderSubSet(const glm::mat4& matrix, const std::vector<int>& indices);

    static GLuint _program;
    GLuint _shapeType;
    GLuint _vertexCount;
    GLuint _u_matrix;
    GLuint _vertexArrayObject;
    GLuint _vertexBufferObject;

};

#endif // QUICKVERTEXBUFFER_H
