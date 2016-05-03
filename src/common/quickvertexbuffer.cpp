#include "quickvertexbuffer.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

const std::string vertexShader(
        "#version 150\n"

        "in vec3 vertex;"

        "uniform mat4 u_matrix;"

        "void main()"
        "{"
        "    gl_Position = u_matrix * vec4(vertex.xyz, 1.0);"
        "}"
    );

const std::string fragmentShader(
        "#version 150\n"

        "out vec4 color;"

        "void main()"
        "{"
        "   color = vec4(0.0, 0.6, 1.0, 1.0);"
        "}"
    );

GLuint QuickVertexBuffer::_program = 0;

QuickVertexBuffer::QuickVertexBuffer(GLuint shapeType, const std::vector<glm::vec3>& vertices)
    : _shapeType(shapeType), _vertexCount(vertices.size()),
      _u_matrix(0), _vertexArrayObject(0), _vertexBufferObject(0)
{
    if (QuickVertexBuffer::_program == 0)
        QuickVertexBuffer::_program = LoadShaderProgram(vertexShader, fragmentShader);
    glUseProgram(QuickVertexBuffer::_program);

    if (this->_u_matrix == 0)
        this->_u_matrix = glGetUniformLocation(QuickVertexBuffer::_program, "u_matrix");

    if (this->_vertexArrayObject == 0)
        glGenVertexArrays(1, &this->_vertexArrayObject);
    glBindVertexArray(this->_vertexArrayObject);

    if (this->_vertexBufferObject == 0)
        glGenBuffers(1, &this->_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), (const GLvoid *)&vertices[0].x, GL_STATIC_DRAW);

    GLint vertexAttrib = glGetAttribLocation(QuickVertexBuffer::_program, "vertex");
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(vertexAttrib);

    glBindVertexArray(0);
}

QuickVertexBuffer::~QuickVertexBuffer()
{ /* TODO: cleanup vertex buffer, vertex array and shader */ }

void QuickVertexBuffer::Render(const glm::mat4& matrix)
{
    glUseProgram(QuickVertexBuffer::_program);
    glUniformMatrix4fv(this->_u_matrix, 1, false, glm::value_ptr(matrix));

    glBindVertexArray(this->_vertexArrayObject);
    glDrawArrays(this->_shapeType, 0, this->_vertexCount);
    glBindVertexArray(0);
}

void QuickVertexBuffer::RenderSubSet(const glm::mat4& matrix, const std::vector<int>& indices)
{
    const int* i = &(indices[0]);
    glUseProgram(QuickVertexBuffer::_program);
    glUniformMatrix4fv(this->_u_matrix, 1, false, glm::value_ptr(matrix));

    glBindVertexArray(this->_vertexArrayObject);
    glDrawElements(this->_shapeType, indices.size(), GL_UNSIGNED_INT, (const GLvoid*)i);
    glBindVertexArray(0);
}


