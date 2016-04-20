#ifndef SHADER_H
#define SHADER_H

#include <GL/glextl.h>
#include <string>

GLuint LoadShaderProgram(const std::string& vertShaderSrc, const std::string& fragShaderSrc);

#endif // SHADER_H
