
#include <cassert>
#include "core/OjyamaPanel.hpp"
#include "core/PanelContainer.hpp"

OjyamaPanel::OjyamaPanel(PanelContainer* panels, int ojyama_id)
    : 
    m_OjyamaInfo(),
    m_PanelContainer(panels)
{
    // 引数のパネルコンテナが不正だったらバグ
    if (!panels) {
        assert(0 && "Argument \'panels\' is null. IT MUST BE BUG");
    }
    
    PanelContainer* panel_field = m_PanelContainer;
    // フィールド上のお邪魔パネルを検索する。
    // 左下の位置を基準とすることとしているので、自分のお邪魔パネルがみつかれば
    // そのお邪魔パネルから情報を取得
    bool is_found = false;
    for (int y = panel_field->FieldBottom(); y <= panel_field->FieldNewOjyamaLine() && !is_found; ++y) {
        for (int x = panel_field->FieldLeft(); x <= panel_field->FieldRight() && !is_found; ++x) {
            const Panel& panel = panel_field->GetPanel(x, y);
            if ( panel.ojyama && panel.ojyama->id == ojyama_id) {
                m_OjyamaInfo = panel.ojyama;
                calculateBasePos(x, y, panel);
                is_found = true;
            }
        }
    }

    // 自分のお邪魔パネルがフィールド上にない場合、is_found は false
    // その場合はバグ。
    if (!is_found) {
        assert(0 && "No OjyamaPanel found. IT MUST BE BUG");
    }
}

void OjyamaPanel::calculateBasePos(int x, int y, const Panel& panel)
{
    m_BasePos.x = x - panel.ojyama_basepos_from.x;
    m_BasePos.y = y - panel.ojyama_basepos_from.y;
}

void OjyamaPanel::SetOjyamaPanel(const Panel& setpanel)
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }

    int w = ojyama->width;
    int h = ojyama->height;
    int start_x = m_BasePos.x;
    int start_y = m_BasePos.y;

    for (int y = start_y; 
         y <= m_PanelContainer->FieldNewOjyamaLine() && y < (start_y + h);
         ++y) {
        for (int x = start_x; 
             x <= m_PanelContainer->FieldRight() && x < (start_x + w);
             ++x) {

            Panel& panel = m_PanelContainer->GetPanel(x, y);
            panel = setpanel;
        }
    }
}

bool OjyamaPanel::IsExistUnderOjyama(Panel::State state, Panel* under_panel)
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }
    if (under_panel == nullptr) {
        return false;
    }

    int w = ojyama->width;
    int start_x = m_BasePos.x;
    bool is_found = false;

    for (int x = start_x;
         x <= m_PanelContainer->FieldRight() && x < (start_x + w);
         ++x) {
        const Panel& p= m_PanelContainer->GetUnderPanel(x, m_BasePos.y);

        if (p.state == state) {
            *under_panel = p;
            is_found = true;
            break;
        }
    }

    return is_found;
}

bool OjyamaPanel::CanFallOjyamaPanel() const
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }

    int left = m_BasePos.x;
    int right = left + ojyama->width;
    bool result = true;

    for (int x = left; x < right; ++x) {
        const Panel& under_panel = m_PanelContainer->GetUnderPanel(x, m_BasePos.y);
        bool can_fall_cond = under_panel.type == Panel::TYPE_SPACE &&
            under_panel.state != Panel::STATE_SWAPPING &&
            under_panel.state != Panel::STATE_DELETE_AFTER_WAIT;

        if (!can_fall_cond) {
            result = false;
        }
    }

    return result;
}

void OjyamaPanel::Fall()
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }

    int w = ojyama->width;
    int h = ojyama->height;
    int start_x = m_BasePos.x;
    int start_y = m_BasePos.y;

    for (int y = start_y;
        y <= m_PanelContainer->FieldNewOjyamaLine() && y < (start_y + h);
        ++y) {
        for (int x = start_x;
            x <= m_PanelContainer->FieldRight() && x < (start_x + w);
            ++x) {

            Panel& under_panel = m_PanelContainer->GetUnderPanel(x, y);
            Panel& panel = m_PanelContainer->GetPanel(x, y);

            panel.fall_before_wait  = 0;
            panel.state             = Panel::STATE_FALLING;
            panel.move_from         = PanelPos(0, 1);
            std::swap(panel, under_panel);
        }
    }
}

void OjyamaPanel::UncompressedBottomLine()
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }

    if( ojyama->height >= 1 ){
        ojyama->height -= 1;
    }
}

void OjyamaPanel::SetUpdated()
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }

    ojyama->is_updated = true;
}

bool OjyamaPanel::IsUpdated() const
{
    auto ojyama = m_OjyamaInfo.lock();
    if (!ojyama) {
        assert(0 && "OjyamaPanel has already deleted. IT MUST BE BUG");
    }

    return ojyama->is_updated;
}
