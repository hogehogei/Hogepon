

#ifndef   EXLIB_EVENT_INTERFACE_HPP
#define   EXLIB_EVENT_INTERFACE_HPP


#include <memory>
#include "HashedString.hpp"


namespace exlib
{


    using EventType = exlib::HashedString;

    /**
     * @brief   �C�x���g���쐬���������́A���̃N���X���p�����邱��
     */
    class IEvent
    {
    public:

        IEvent() {}
        virtual ~IEvent() noexcept {}

        //! �C�x���g�^�C�v�擾
        virtual const EventType& EventType() const = 0;
    };

    
    using IEventPtr = std::shared_ptr<IEvent>;

}


#endif
