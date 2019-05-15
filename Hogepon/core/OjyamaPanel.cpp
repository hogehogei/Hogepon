
#include <cassert>
#include "core/OjyamaPanel.hpp"
#include "core/PanelContainer.hpp"

OjyamaPanel::OjyamaPanel(PanelContainer* container, int ojyama_id)
    : 
    m_OjyamaInfo(),
    m_PanelContainer(container)
{
    // �����̃p�l���R���e�i���s����������o�O
    if (!container) {
        assert(0 && "Argument \'panels\' is null. IT MUST BE BUG");
    }
    
    PanelContainer* panel_field = m_PanelContainer;
    // �t�B�[���h��̂��ז��p�l������������B
    // �����̈ʒu����Ƃ��邱�ƂƂ��Ă���̂ŁA�����̂��ז��p�l�����݂����
    // ���̂��ז��p�l����������擾
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

    // �����̂��ז��p�l�����t�B�[���h��ɂȂ��ꍇ�Ais_found �� false
    // ���̏ꍇ�̓o�O�B
    if (!is_found) {
        assert(0 && "No OjyamaPanel found. IT MUST BE BUG");
    }
}

OjyamaPanel::OjyamaPanel(PanelContainer* container, int x, int y, const Panel& panel)
    :
    m_OjyamaInfo(),
    m_PanelContainer(container)
{
    // �����̃p�l���R���e�i���s����������o�O
    if (!container) {
        assert(0 && "Argument \'panels\' is null. IT MUST BE BUG");
    }

    if (panel.type == Panel::TYPE_OJYAMA && panel.ojyama) {
        m_OjyamaInfo = panel.ojyama;
        calculateBasePos(x, y, panel);
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
            // �J�E���^�Ə�Ԃ����Z�b�g
            panel.move_from          = setpanel.move_from;
            panel.delete_before_wait = setpanel.delete_before_wait;
            panel.delete_wait        = setpanel.delete_wait;
            panel.delete_after_wait  = setpanel.delete_after_wait;
            panel.swapping_count     = setpanel.swapping_count;
            panel.fall_before_wait   = setpanel.fall_before_wait;
            panel.fall_count         = setpanel.fall_count;
            panel.fall_after_wait    = setpanel.fall_after_wait;
            panel.state              = setpanel.state;
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

void OjyamaPanel::UncompressBottomLine()
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

            if (panel.ojyama_basepos_from.y >= 1) {
                panel.ojyama_basepos_from.y -= 1;
                panel.is_mark_be_panel = false;
            }
            else {
                panel.is_mark_be_panel = true;
            }
        }
    }

    if (ojyama->height >= 1) {
        ojyama->height -= 1;
    }
}

const Panel& OjyamaPanel::GetBasePanel() const
{
    return m_PanelContainer->GetPanel(m_BasePos.x, m_BasePos.y);
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
