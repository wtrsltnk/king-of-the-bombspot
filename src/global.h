#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <glm/glm.hpp>

std::ostream& operator << (std::ostream& os, const glm::vec2& v);
std::ostream& operator << (std::ostream& os, const glm::vec3& v);
std::ostream& operator << (std::ostream& os, const glm::vec4& v);

extern class Font cs;
extern class Font small;

extern glm::mat4 orth;

#define ENABLE_LOG 0

#if ENABLE_LOG
#define LOG(msg) std::cout << msg << std::endl
#else
#define LOG(msg)
#endif

#endif // GLOBAL_H
