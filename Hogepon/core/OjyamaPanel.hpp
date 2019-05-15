#pragma once

#include <memory>
#include "core/Panel.hpp"

class PanelContainer;
class OjyamaPanel
{
public:

    // 新規作成は不許可。コピーのみ許可
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

    // PanelContainer class からのみ作成可能。
    friend class PanelContainer;
    OjyamaPanel(PanelContainer* container, int ojyama_id);
    OjyamaPanel(PanelContainer* container, int x, int y, const Panel& panel);

    void calculateBasePos(int x, int y, const Panel& panel);

    std::weak_ptr<Ojyama> m_OjyamaInfo;
    PanelContainer*       m_PanelContainer;
    PanelPos              m_BasePos;
};