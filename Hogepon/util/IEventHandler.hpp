
#ifndef   EXLIB_EVENT_HANDER_INTERFACE_HPP
#define   EXLIB_EVENT_HANDER_INTERFACE_HPP

#include <memory>

namespace exlib
{


    // Forward declaration
    class IEvent;
    class HashedString;

    /**
     * @brief   イベントハンドラを作成したい時は、このクラスを継承すること
     **/
    class IEventHandler
    {
    public:

        IEventHandler() {}
        virtual ~IEventHandler() noexcept {}

        /**
         * @brief   イベントを処理する
         *
         * イベントを処理する内容はこの関数をオーバーライドして
         * その中に書くこと
         */
        virtual bool HandleEvent(const IEvent& event) = 0;

        /**
         * @brief    優先度を取得
         */
        virtual int Priority() const { return skDefaultPriority; }

        /**
         * @brief    ハンドラ識別用の名前を付ける
         */
        virtual const HashedString& Name() const = 0;

    private:

        static const int skDefaultPriority = 10000;
    };




    template <typename T>
    class EventHandlerBase : public IEventHandler
    {
    public:

        EventHandlerBase(const HashedString& ident_str, T* handler_class)
        : m_IdentStr(ident_str),
          m_HandlerClass(handler_class)
        {}

        virtual const HashedString& Name() const { return m_IdentStr; }
        virtual bool HandleEvent(const IEvent& event)
        {
            return T::EventHandler(m_HandlerClass, event);
        }

    private:

        HashedString m_IdentStr;
        T* m_HandlerClass;
    };

    using IEventHandlerPtr = std::shared_ptr<IEventHandler>;

}

#endif
