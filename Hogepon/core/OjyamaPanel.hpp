#pragma once

#include <memory>
#include "core/Panel.hpp"

class PanelContainer;
class OjyamaPanel
{
public:

    // �V�K�쐬�͕s���B�R�s�[�̂݋���
    OjyamaPanel() = delete;

    void SetOjyamaPanel(const Panel& setpanel);
    bool IsExistUnderOjyama(Panel::State state, Panel* under_panel);
    bool CanFallOjyamaPanel() const;
    void Fall();
    void UncompressBottomLine();

    const Panel& GetBasePanel() const;
    void SetUpdated();
    bool IsUpdated() const;

private:

    // PanelContainer class ����̂ݍ쐬�\�B
    friend class PanelContainer;
    OjyamaPanel(PanelContainer* container, int x, int y);

    std::weak_ptr<Ojyama> m_OjyamaInfo;
    PanelContainer*       m_PanelContainer;
    PanelPos              m_BasePos;
};


class OjyamaInfo
{
public:

    enum Part {
        OneLine_Left,
        OneLine_Center,
        OneLine_Right,
        MultiLine_TopLeft,
        MultiLine_TopCenter,
        MultiLine_TopRight,
        MultiLine_MiddleLeft,
        MultiLine_MiddleCenter,
        MultiLine_MiddleRight,
        MultiLine_BottomLeft,
        MultiLine_BottomCenter,
        MultiLine_BottomRight,
        OutRange,
    };

public:

    // �V�K�쐬�͕s���B�R�s�[�̂݋���
    OjyamaInfo() = delete;

    Part GetPart(int x, int y) const;

private:

    // PanelContainer class ����̂ݍ쐬�\�B
    friend class PanelContainer;
    OjyamaInfo(const PanelContainer* container, int x, int y);

    std::weak_ptr<Ojyama> m_OjyamaInfo;
    const PanelContainer* m_PanelContainer;
    PanelPos              m_BasePos;
};