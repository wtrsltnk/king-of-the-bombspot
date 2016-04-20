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

QuickVertexBuffer::QuickVertexBuffer(GLuint shapeType, const std::vector<glm::vec3>& vertices)
    : _shapeType(shapeType), _vertexCount(vertices.size()),
      _program(0), _u_matrix(0), _vertexArrayObject(0), _vertexBufferObject(0)
{
    if (this->_program == 0)
        this->_program = LoadShaderProgram(vertexShader, fragmentShader);
    glUseProgram(this->_program);

    if (this->_u_matrix == 0)
        this->_u_matrix = glGetUniformLocation(this->_program, "u_matrix");

    if (this->_vertexArrayObject == 0)
        glGenVertexArrays(1, &this->_vertexArrayObject);
    glBindVertexArray(this->_vertexArrayObject);

    if (this->_vertexBufferObject == 0)
        glGenBuffers(1, &this->_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), (const GLvoid *)&vertices[0].x, GL_STATIC_DRAW);

    GLint vertexAttrib = glGetAttribLocation(this->_program, "vertex");
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(vertexAttrib);

    glBindVertexArray(0);
}

QuickVertexBuffer::~QuickVertexBuffer()
{ /* TODO: cleanup vertex buffer, vertex array and shader */ }

void QuickVertexBuffer::Render(const glm::mat4& matrix)
{
    glUseProgram(this->_program);
    glUniformMatrix4fv(this->_u_matrix, 1, false, glm::value_ptr(matrix));

    glBindVertexArray(this->_vertexArrayObject);
    glDrawArrays(this->_shapeType, 0, this->_vertexCount);
    glBindVertexArray(0);
}

