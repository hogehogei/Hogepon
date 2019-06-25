#pragma once

#include <Siv3D.hpp>


class PanelDeleteEffect
{
public:

    explicit PanelDeleteEffect(int xpos, int ypos, double radian, double speed, int lifetime_ms, double effect_size);

    void Update();
    void Draw(int pixel_x, int pixel_y);
    bool IsFinished() const;
    int PosX() const;
    int PosY() const;

private:

    int m_PanelPosX;
    int m_PanelPosY;
    Vec2 m_Vec;
    Vec2 m_Move;
    double m_Speed;
    int m_LifeTimeMs;
    double m_EffectSize;

    s3d::Stopwatch m_StopWatch;
};