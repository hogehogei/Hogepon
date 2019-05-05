
#include "core/OjyamaCreator.hpp"
#include "core/PanelContainer.hpp"

void OjyamaCreator::Append(int chain_count, int doujikeshi_count)
{
    if (chain_count >= 2) {
        queue_ChainOjyama(chain_count);
    }
    if (doujikeshi_count >= 4) {
        queue_DoujikeshiOjyama(doujikeshi_count);
    }
}

void OjyamaCreator::FallOjyamaLine(PanelContainer* container, bool dequeue_enable)
{
    if (dequeue_enable && !m_OjyamaNext.ojyama) {
        if (!m_OjyamaPending.empty()) {
            m_OjyamaNext = m_OjyamaPending.front();
            m_OjyamaPending.pop();
        }
    }

    if (!m_OjyamaNext.ojyama) {
        return;
    }
    if (isExistOjyamaInNewOjyamaLine(container)) {
        return;
    }

    auto ojyama_ptr = m_OjyamaNext.ojyama;
    int left = container->FieldLeft();
    for (int x = left; x < left + ojyama_ptr->width; ++x) {
        Panel& panel = container->GetPanel(x, container->FieldNewOjyamaLine());
        panel.type = Panel::TYPE_OJYAMA;
        panel.state = Panel::STATE_DEFAULT;
        panel.ojyama = ojyama_ptr;
        panel.ojyama_basepos_from.y = ojyama_ptr->height - m_OjyamaNext.remain_line;
        panel.ojyama_basepos_from.x = x;
    }

    --m_OjyamaNext.remain_line;
    if (m_OjyamaNext.remain_line <= 0) {
        m_OjyamaNext.ojyama.reset();
    }
}

bool OjyamaCreator::isExistOjyamaInNewOjyamaLine(const PanelContainer* container)
{
    bool is_ojyama_exist = false;
    auto ojyama_ptr = m_OjyamaNext.ojyama;
    int left = container->FieldLeft();

    for (int x = left; x < left + ojyama_ptr->width; ++x) {
        const Panel& panel = container->GetPanel(x, container->FieldNewOjyamaLine());
        if (panel.type != Panel::TYPE_SPACE) {
            is_ojyama_exist = true;
        }
    }

    return is_ojyama_exist;
}

void OjyamaCreator::queue_ChainOjyama(int chain_count)
{
    std::shared_ptr<Ojyama> ojyama_ptr(std::make_shared<Ojyama>());
    ojyama_ptr->height     = chain_count - 1;
    ojyama_ptr->width      = 6;
    ojyama_ptr->is_updated = false;
    ojyama_ptr->id         = ++m_OjyamaID;

    if (!m_OjyamaNext.ojyama) {
        m_OjyamaNext.ojyama = ojyama_ptr;
        m_OjyamaNext.remain_line = ojyama_ptr->height;
    }
    else {
        OjyamaFall o;
        o.ojyama = ojyama_ptr;
        o.remain_line = ojyama_ptr->height;
        m_OjyamaPending.push(o);
    }
}

void OjyamaCreator::queue_DoujikeshiOjyama(int doujikeshi_count)
{
}