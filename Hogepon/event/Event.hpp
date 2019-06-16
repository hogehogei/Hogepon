#pragma once

#include "util/IEvent.hpp"
#include "util/HashedString.hpp"


bool RegisterAllEvent();


struct OjyamaFallEvent : public exlib::IEvent
{
    static const inline exlib::EventType sk_EventType = "OjyamaFallEvent";
    virtual const exlib::EventType& EventType() const override
    {
        return sk_EventType;
    }
};


