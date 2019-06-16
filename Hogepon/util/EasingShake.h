#pragma once

#include <Siv3D.hpp>

class EasingShake
{
public:

    EasingShake()
        :
        m_Amptitude(0.0),
        m_Hz(0.0),
        m_TimeMs(0)
    {}

    EasingShake(double amptitude, int hz, uint64_t time_ms)
        :
        m_Amptitude(amptitude),
        m_Hz(hz),
        m_TimeMs(time_ms)
    {}

    void Start()
    {
        m_StopWatch = s3d::Stopwatch(true);
    }

    void Stop()
    {
        m_StopWatch.pause();
    }

    double Amptitude()
    {
        uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_StopWatch.elapsed()).count();
        if (elapsed_ms > m_TimeMs) {
            m_StopWatch.pause();
            elapsed_ms = m_TimeMs;
        }

        double elapsed_ms_double = elapsed_ms;
        double elapsed_t = elapsed_ms_double / m_TimeMs;
        double amptitude_attenuation = (1.0 - elapsed_t);

        // éwíËÇµÇΩé¸îgêîÇÃsineîgå`Ç… êUïùÇä|ÇØÇÈÅB
        return s3d::Sin(m_Hz * (elapsed_ms_double / 1000.0) * 2.0 * s3d::Math::Pi) * (m_Amptitude * amptitude_attenuation);
    }

private:

    double m_Amptitude;
    double m_Hz;
    uint64_t m_TimeMs;

    s3d::Stopwatch m_StopWatch;
};