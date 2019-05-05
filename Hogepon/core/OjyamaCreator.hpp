#pragma once

#include <queue>
#include <memory>
#include "core/Panel.hpp"

struct OjyamaFall
{
    std::shared_ptr<Ojyama> ojyama;
    int remain_line;
};

class PanelContainer;
class OjyamaCreator
{
public:

    OjyamaCreator() = default;
    ~OjyamaCreator() noexcept = default;

    void Append(int chain_count, int doujikeshi_count);
    void FallOjyamaLine(PanelContainer* container, bool dequeue_enable);

private:

    bool isExistOjyamaInNewOjyamaLine(const PanelContainer* container);
    void queue_ChainOjyama(int chain_count);
    void queue_DoujikeshiOjyama(int doujikeshi_count);

    std::queue<OjyamaFall> m_OjyamaPending;
    OjyamaFall             m_OjyamaNext;
    uint32_t               m_OjyamaID;
};
