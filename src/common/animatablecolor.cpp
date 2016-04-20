#include "animatablecolor.h"
#include "../global.h"

AnimatableColor::AnimatableColor() : _duration(0.0f) { }
AnimatableColor::AnimatableColor(const glm::vec4 &color) : _nextColor(color) { (*this) = color; }
AnimatableColor::~AnimatableColor() { }

AnimatableColor& AnimatableColor::operator = (const glm::vec4& color)
{
    this->r = color.r;
    this->g = color.g;
    this->b = color.b;
    this->a = color.a;

    return *this;
}

AnimatableColor& AnimatableColor::operator = (const glm::vec2& v2)
{
    this->r = v2.r;
    this->g = v2.g;
    this->b = 0;
    this->a = 0;

    return *this;
}

void AnimatableColor::ChangeTo(const glm::vec4& nextColor, float duration)
{
    this->_nextColor = nextColor;
    this->_duration = duration;
}

void AnimatableColor::Update(float dt)
{
    if (this->_duration > 0.0f)
    {
        float r = this->_nextColor.r - this->r;
        this->r += (r / this->_duration) * dt;

        float g = this->_nextColor.g - this->g;
        this->g += (g / this->_duration) * dt;

        float b = this->_nextColor.b - this->b;
        this->b += (b / this->_duration) * dt;

        float a = this->_nextColor.a - this->a;
        this->a += (a / this->_duration) * dt;

        this->_duration -= dt;
    }
}
