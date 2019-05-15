#pragma once

#include <memory>
#include "core/Panel.hpp"

class PanelContainer;
class OjyamaPanel
{
public:

    // �V�K�쐬�͕s���B�R�s�[�̂݋���
    OjyamaPanel() = delete;
    OjyamaPanel(const OjyamaPanel&) = default;

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
    OjyamaPanel(PanelContainer* container, int ojyama_id);
    OjyamaPanel(PanelContainer* container, int x, int y, const Panel& panel);

    void calculateBasePos(int x, int y, const Panel& panel);

    std::weak_ptr<Ojyama> m_OjyamaInfo;
    PanelContainer*       m_PanelContainer;
    PanelPos              m_BasePos;
};