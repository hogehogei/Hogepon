

#ifndef   EXLIB_EVENT_INTERFACE_HPP
#define   EXLIB_EVENT_INTERFACE_HPP


#include <memory>
#include "HashedString.hpp"


namespace exlib
{


    using EventType = exlib::HashedString;

    /**
     * @brief   イベントを作成したい時は、このクラスを継承すること
     */
    class IEvent
    {
    public:

        IEvent() {}
        virtual ~IEvent() noexcept {}

        //! イベントタイプ取得
        virtual const EventType& EventType() const = 0;
    };

    
    using IEventPtr = std::shared_ptr<IEvent>;

}


#endif
