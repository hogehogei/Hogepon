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

struct DoujikeshiEvent : public exlib::IEvent
{
    static const inline exlib::EventType sk_EventType = "DoujikeshiEvent";
    
    DoujikeshiEvent(int doujikeshi_count)
        : doujikeshiCount(doujikeshi_count)
    {}
    
    virtual const exlib::EventType& EventType() const override
    {
        return sk_EventType;
    }

    int doujikeshiCount;
};

struct PanelDeletedEvent : public exlib::IEvent
{
    static const inline exlib::EventType sk_EventType = "PanelDeletedEvent";

    PanelDeletedEvent(int xpos, int ypos)
        : 
        xPos(xpos),
        yPos(ypos)
    {}

    virtual const exlib::EventType& EventType() const override
    {
        return sk_EventType;
    }

    int xPos;
    int yPos;
};

