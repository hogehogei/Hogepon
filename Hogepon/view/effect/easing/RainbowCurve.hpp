#pragma once

#include <Siv3D.hpp>

namespace easing
{
    class RainbowCurve
    {
    public:

        RainbowCurve(uint32_t period_ms)
            : 
            m_PeriodMs(period_ms)
        {}

        double Out( uint64_t elapsed_ms ) const
        {
            double ret = 0.0;
            uint64_t half_period = m_PeriodMs / 2;
            uint64_t elapsed = elapsed_ms % m_PeriodMs;
            double blink_t = static_cast<double>(elapsed % half_period) / half_period;

            if (elapsed < (m_PeriodMs / 2)) {
                ret = s3d::Math::Exp(-1 * blink_t * 2.0);
            }
            else {
                ret = s3d::Math::Exp(-1 * (1.0 - blink_t) * 2.0);
            }

            return ret;
        }

    private:

        const uint32_t m_PeriodMs;
    };
}