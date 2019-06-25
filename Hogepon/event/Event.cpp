
#include "event/Event.hpp"
#include "util/EventManager.hpp"

bool RegisterAllEvent()
{
    exlib::EventManager& manager = exlib::EventManager::Instance();

    // “o˜^‚µ‚½‚¢ƒCƒxƒ“ƒg‚ð—…—ñ‚·‚é
    manager.RegisterEvent(OjyamaFallEvent::sk_EventType);
    manager.RegisterEvent(DoujikeshiEvent::sk_EventType);
    manager.RegisterEvent(PanelDeletedEvent::sk_EventType);

    return true;
}