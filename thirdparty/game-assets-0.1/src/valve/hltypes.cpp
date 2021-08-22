#include "valve/hltypes.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace valve;

VertexBuffer::VertexBuffer()
    : _vao(0), _vbo(0)
{}

VertexBuffer::~VertexBuffer()
{
    //    this->_faces.Delete();
    //    this->_textures.Delete();
    //    while (this->_lightmaps.empty() == false)
    //    {
    //        Texture* t = this->_lightmaps.back();
    //        this->_lightmaps.pop_back();
    //        delete t;
    //    }
}

void VertexBuffer::LoadVertices(const std::vector<tVertex> &vertices)
{
    if (vertices.size() > 0)
    {
        if (this->_vao == 0)
            glGenVertexArrays(1, &this->_vao);
        glBindVertexArray(this->_vao);

        if (this->_vbo == 0)
            glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(tVertex), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(tVertex), (GLvoid *)&vertices[0]);

        glVertexAttribPointer((GLuint)ShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), 0);
        glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::Vertex);

        glVertexAttribPointer((GLuint)ShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid *)(sizeof(float) * 3));
        glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::TexCoord);

        glVertexAttribPointer((GLuint)ShaderAttributeLocations::LightCoord, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid *)(sizeof(float) * 5));
        glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::LightCoord);

        glVertexAttribPointer((GLuint)ShaderAttributeLocations::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid *)(sizeof(float) * 7));
        glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::Normal);

        glVertexAttribPointer((GLuint)ShaderAttributeLocations::Bone, 1, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid *)(sizeof(float) * 10));
        glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::Bone);

        glBindVertexArray(0);
    }
}

void VertexBuffer::RenderFaces(const std::set<unsigned short> &visibleFaces, GLenum mode)
{
    Bind();

    for (std::set<unsigned short>::const_iterator i = visibleFaces.begin(); i != visibleFaces.end(); ++i)
    {
        short a = *i;

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_lightmaps[this->_faces[a].lightmap]->GlIndex());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_textures[this->_faces[a].texture]->GlIndex());

        glDrawArrays(mode, this->_faces[a].firstVertex, this->_faces[a].vertexCount);
    }

    Unbind();
}

void VertexBuffer::Bind()
{
    glBindVertexArray(this->_vao);
}

void VertexBuffer::Unbind()
{
    glBindVertexArray(0);
}

GLuint LoadShaderProgram(const std::string &vertShaderStr, const std::string &fragShaderStr, const std::map<ShaderAttributeLocations, std::string> &attrLoc)
{
    if (glCreateShader == 0) std::cout << "glCreateShader not loaded" << std::endl;
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
        std::cout << &vertShaderError[0] << std::endl;
    }

    // Compile fragment shader
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
        std::cout << &fragShaderError[0] << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    for (auto i = attrLoc.begin(); i != attrLoc.end(); ++i)
        glBindAttribLocation(program, int(i->first), i->second.c_str());

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError((logLength > 1) ? logLength : 1);
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        std::cout << &programError[0] << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

Shader::Shader()
    : _program(0)
{}

Shader::~Shader()
{}

const std::map<ShaderAttributeLocations, std::string> Shader::AttribLocations()
{
    std::map<ShaderAttributeLocations, std::string> attrLoc;

    attrLoc.insert(std::make_pair(ShaderAttributeLocations::Vertex, "vertex"));
    attrLoc.insert(std::make_pair(ShaderAttributeLocations::Normal, "normal"));
    attrLoc.insert(std::make_pair(ShaderAttributeLocations::TexCoord, "texcoords"));
    attrLoc.insert(std::make_pair(ShaderAttributeLocations::LightCoord, "lightcoords"));
    attrLoc.insert(std::make_pair(ShaderAttributeLocations::Bone, "bone"));

    return attrLoc;
}

void Shader::BuildProgram()
{
    this->_program = LoadShaderProgram(this->VertexShader(), this->FragmentShader(), this->AttribLocations());
    glUseProgram(this->_program);

    this->_u_projection = glGetUniformLocation(this->_program, "u_projection");
    this->_u_view = glGetUniformLocation(this->_program, "u_view");

    this->_u_light = glGetUniformLocation(this->_program, "u_light");
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(this->_u_light, 1);

    this->_u_tex = glGetUniformLocation(this->_program, "u_tex");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(this->_u_tex, 0);

    this->_u_bones = 0;
    GLint uniform_block_index = glGetUniformBlockIndex(this->_program, "u_bones");
    glUniformBlockBinding(this->_program, uniform_block_index, this->_u_bones);

#define MAX_MDL_BONES 128
    glGenBuffers(1, &this->_bonesBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, this->_bonesBuffer);
    glBufferData(GL_UNIFORM_BUFFER, MAX_MDL_BONES * sizeof(glm::mat4), 0, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    this->OnProgramLinked(this->_program);
}

void Shader::UseProgram()
{
    glUseProgram(this->_program);
}

void Shader::SetProjectionMatrix(const glm::mat4 &m)
{
    glUniformMatrix4fv(this->_u_projection, 1, false, glm::value_ptr(m));
}

void Shader::SetViewMatrix(const glm::mat4 &m)
{
    glUniformMatrix4fv(this->_u_view, 1, false, glm::value_ptr(m));
}

void Shader::BindBones(const glm::mat4 m[], int count)
{
    glBindBuffer(GL_UNIFORM_BUFFER, this->_bonesBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, count * sizeof(glm::mat4), glm::value_ptr(m[0]));
    glBindBufferRange(GL_UNIFORM_BUFFER, this->_u_bones, this->_bonesBuffer, 0, count * sizeof(glm::mat4));
}

void Shader::UnbindBones()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

const std::string Shader::VertexShader()
{
    return std::string(
        "#version 150\n"

        "in vec3 vertex;"
        "in vec3 normal;"
        "in vec2 texcoords;"
        "in vec2 lightcoords;"
        "in int bone;"

        "uniform mat4 u_projection;"
        "uniform mat4 u_view;"
        "uniform BonesBlock"
        "{"
        "   mat4 u_bones[32];"
        "};"

        "out vec2 f_uv_tex;"
        "out vec2 f_uv_light;"

        "void main()"
        "{"
        "    mat4 m = u_projection * u_view;"
        "    if (bone >= 0) m = m; * u_bones[bone];"
        "    gl_Position = m * vec4(vertex.xyz, 1.0);"
        "    f_uv_tex = texcoords;"
        "    f_uv_light = lightcoords;"
        "}");
}

const std::string Shader::FragmentShader()
{
    return std::string(
        "#version 150\n"

        "uniform sampler2D u_tex;"
        "uniform sampler2D u_light;"

        "in vec2 f_uv_tex;"
        "in vec2 f_uv_light;"

        "out vec4 color;"

        "void main()"
        "{"
        "    color = texture(u_tex, f_uv_tex.st) * texture(u_light, f_uv_light.st);"
        "}");
}
