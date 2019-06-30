#pragma once

#include <Siv3D.hpp>

namespace easing
{
    class Shake
    {
    public:

        Shake()
            :
            m_Amptitude(0.0),
            m_Hz(0.0),
            m_TimeMs(0)
        {}

        Shake(double amptitude, int hz, uint64_t time_ms)
            :
            m_Amptitude(amptitude),
            m_Hz(hz),
            m_TimeMs(time_ms)
        {}

        double Amptitude( uint64_t elapsed_ms )
        {
            if (elapsed_ms > m_TimeMs) {
                elapsed_ms = m_TimeMs;
            }

            double elapsed_ms_double = elapsed_ms;
            double elapsed_t = elapsed_ms_double / m_TimeMs;
            double amptitude_attenuation = (1.0 - elapsed_t);

            // w’è‚µ‚½ü”g”‚Ìsine”gŒ`‚É U•‚ğŠ|‚¯‚éB
            return s3d::Sin(m_Hz * (elapsed_ms_double / 1000.0) * 2.0 * s3d::Math::Pi) * (m_Amptitude * amptitude_attenuation);
        }

    private:

        double m_Amptitude;
        double m_Hz;
        uint64_t m_TimeMs;
    };
}
