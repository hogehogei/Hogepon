#pragma once

#include "util/IEvent.hpp"
#include "util/HashedString.hpp"

#include "core/Panel.hpp"

struct OjyamaFallEvent : public exlib::IEvent
{
    static const inline exlib::EventType sk_EventType = "OjyamaFallEvent";
    virtual const exlib::EventType& EventType() const override
    {
        return sk_EventType;
    }
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

class OjyamaAttackEvent : public exlib::IEvent
{
public:

    static const inline exlib::EventType sk_EventType = "OjyamaAttackEvent";

    OjyamaAttackEvent(int doujikeshi_count, int chain_count, const PanelPos& top_panel_pos)
        :
        m_DoujikeshiCount(doujikeshi_count),
        m_ChainCount(chain_count),
        m_TopPanelPos(top_panel_pos)
    {}

    int DoujikeshiCount() const { return m_DoujikeshiCount; }
    int ChainCount() const { return m_ChainCount; }
    const PanelPos& TopPanelPos() const { return m_TopPanelPos; }

    virtual const exlib::EventType& EventType() const override
    {
        return sk_EventType;
    }

private:

    const int m_DoujikeshiCount;
    const int m_ChainCount;
    const PanelPos m_TopPanelPos;
};
