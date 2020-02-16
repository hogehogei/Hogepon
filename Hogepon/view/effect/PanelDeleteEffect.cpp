
#include "view/effect/PanelDeleteEffect.hpp"

PanelDeleteEffect::PanelDeleteEffect(int xpos, int ypos, double radian, double speed, int lifetime_ms, double effect_size)
    :
    m_PanelPosX(xpos),
    m_PanelPosY(ypos),
    m_Vec(s3d::Cos(radian), s3d::Sin(radian)),
    m_Move(0.0, 0.0),
    m_Speed(speed),
    m_LifeTimeMs(lifetime_ms),
    m_EffectSize(effect_size),
    m_StopWatch(true)
{}

void PanelDeleteEffect::Update()
{
    // ストップウォッチを更新する
    uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_StopWatch.elapsed()).count();
    if (elapsed_ms > m_LifeTimeMs) {
        m_StopWatch.pause();
    }

    // 現在位置に移動ベクトルを足す
    m_Move.x += m_Vec.x * m_Speed;
    m_Move.y += m_Vec.y * m_Speed;
}

void PanelDeleteEffect::Draw( int pixel_x, int pixel_y )
{
    uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_StopWatch.elapsed()).count();
    if (elapsed_ms > m_LifeTimeMs) {
        elapsed_ms = m_LifeTimeMs;
    }

    double t = (static_cast<double>(elapsed_ms) / m_LifeTimeMs);
    Vec2 drawpos(pixel_x + m_Move.x, pixel_y + m_Move.y);
    double drawsize = (1.0 - t) * m_EffectSize;

    s3d::RenderStateBlock2D blend(BlendState::Additive);
    s3d::Circle(drawpos, drawsize / 2.0).drawShadow(Vec2(0, 0), 3.0, 0, s3d::ColorF(s3d::Palette::Yellow));
    s3d::Circle(drawpos, drawsize).drawShadow(Vec2(0, 0), 6.0, 0, s3d::ColorF(s3d::Palette::Yellow));
}

bool PanelDeleteEffect::IsFinished() const
{
    return m_StopWatch.isPaused();
}

int PanelDeleteEffect::PosX() const
{
    return m_PanelPosX;
}

int PanelDeleteEffect::PosY() const
{
    return m_PanelPosY;
}