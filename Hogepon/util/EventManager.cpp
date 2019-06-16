

#include <chrono>
#include <algorithm>
#include "EventManager.hpp"

// unnamed namespace
namespace
{

    class CompareHandler
    {
    public:
        CompareHandler(const exlib::HashedString& handler_name)
            : mHandlerName(handler_name) {}

        bool operator()(const exlib::IEventHandlerPtr& v) {
            return v->Name() == mHandlerName;
        }

    private:
        const exlib::HashedString mHandlerName;
    };

}


namespace exlib
{

    EventManager::EventManager()
    {}

    EventManager::~EventManager()
    {}

    EventManager& EventManager::Instance()
    {
        // C++03���ƃX���b�h�Z�[�t����Ȃ����ǁAC++0x�Ȃ�X���b�h�Z�[�t
        static exlib::EventManager manager;
        return manager;
    }

    bool EventManager::IsValidEvent(const EventType& type) const
    {
        return std::binary_search(m_RegisteredEvent.begin(), m_RegisteredEvent.end(), type);
    }

    bool EventManager::RegisterEvent(const EventType& type)
    {
        // ���ɓo�^����Ă���Ζ���
        if (IsValidEvent(type)) {
            return false;
        }

        EventTypeVector& events = m_RegisteredEvent;
        events.push_back(type);

        // insertion sort
        // ����C�x���g�����肷��̂ɂQ���T���g������
        int i = events.size() - 1;
        while (i > 0 && events[i - 1] > type) {
            events[i] = events[i - 1];
            --i;
        }
        events[i] = type;

        return true;
    }

    bool EventManager::AddHandler(const EventType& type, const IEventHandlerPtr& handler)
    {
        if (!IsValidEvent(type)) {
            return false;
        }
        EventHandlerVector& handler_vec = m_HandlerMappingTable[type];
        handler_vec.push_back(handler);

        // insertion sort
        int i = handler_vec.size() - 1;
        while (i > 0 && handler_vec[i - 1]->Priority() > handler->Priority()) {
            handler_vec[i] = handler_vec[i - 1];
            --i;
        }
        handler_vec[i] = handler;

        return true;
    }

    bool EventManager::RemoveHandler(const HashedString& handler_name)
    {
        bool result = false;
        CompareHandler pred(handler_name);

        for (EventHandlerTable::iterator itr = m_HandlerMappingTable.begin(), end = m_HandlerMappingTable.end();
            itr != end;
            ++itr) {
            EventHandlerVector& handler_vec = itr->second;

            EventHandlerVector::iterator hander_vec_end = handler_vec.end();
            EventHandlerVector::iterator erase_begin = std::remove_if(handler_vec.begin(),
                hander_vec_end,
                pred);
            if (erase_begin != hander_vec_end) {
                result = true;
            }
            handler_vec.erase(erase_begin, hander_vec_end);
        }

        return result;
    }

    bool EventManager::QueueEvent(const IEventPtr& event)
    {
        if (!IsValidEvent(event->EventType())) {
            return false;
        }
        m_EventQueue[m_CurrentQueueNum].push(event);

        return true;
    }

    bool EventManager::SendEvent(const IEventPtr& event)
    {
        if (!IsValidEvent(event->EventType())) {
            return false;
        }

        EventHandlerVector& handler_vec = m_HandlerMappingTable[event->EventType()];
        for (EventHandlerVector::iterator itr = handler_vec.begin(), end = handler_vec.end();
            itr != end;
            ++itr) {
            (*itr)->HandleEvent(*event);
        }

        return true;
    }

    bool EventManager::Update(uint64_t time_limit_ms, bool is_ignore_events)
    {
        // �g�p����L���[�����ւ���
        EventQueue& event_queue = m_EventQueue[m_CurrentQueueNum];
        m_CurrentQueueNum += 1;
        m_CurrentQueueNum = m_CurrentQueueNum % sk_EventQueueNum;

        std::chrono::system_clock::time_point  start, now;
        start = std::chrono::system_clock::now(); // �v���J�n����
        now = std::chrono::system_clock::now();   // �������܂̎���

        // �����ɗv��������
        uint64_t elapsed = 0;

        while (!event_queue.empty() && elapsed < time_limit_ms) {
            IEventPtr event = event_queue.front();
            event_queue.pop();

            EventHandlerVector handler_vec = m_HandlerMappingTable[event->EventType()];
            for (EventHandlerVector::iterator itr = handler_vec.begin(), end = handler_vec.end();
                itr != end;
                ++itr) {
                (*itr)->HandleEvent(*event);
            }

            //�����ɗv�������Ԃ��~���b�ɕϊ�
            now = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        }


        bool is_all_events_processed = true;
        if (!event_queue.empty() && !is_ignore_events) {
            is_all_events_processed = false;
            // ���ԓ��ɏ����ł��Ȃ������̂ŁAFIFO�Ŏ��̃L���[�Ɉڂ��ւ�
            while (!event_queue.empty()) {
                m_EventQueue[m_CurrentQueueNum].push(event_queue.front());
                event_queue.pop();
            }
        }

        return is_all_events_processed;
    }

}

