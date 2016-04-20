#ifndef ANIMATABLECOLOR_H
#define ANIMATABLECOLOR_H

#include <glm/glm.hpp>

#define RGBA(r, g, b, a) glm::vec4(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f)

class AnimatableColor : public glm::vec4
{
private:
    glm::vec4 _nextColor;
    float _duration;

public:
    AnimatableColor();
    AnimatableColor(const glm::vec4& color);
    virtual ~AnimatableColor();

    AnimatableColor& operator = (const glm::vec4& color);
    AnimatableColor& operator = (const glm::vec2& v2);
    void ChangeTo(const glm::vec4& nextColor, float duration = 0.0f);
    void Update(float dt);

    glm::vec2 ToVec2() { return glm::vec2(this->x, this->y); }
};

#endif // ANIMATABLECOLOR_H
