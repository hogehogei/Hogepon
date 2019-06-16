
#ifndef   EXLIB_EVENT_MANAGER_HPP
#define   EXLIB_EVENT_MANAGER_HPP


#include <vector>
#include <queue>
#include <map>

#include "IEvent.hpp"
#include "IEventHandler.hpp"


namespace exlib
{

class EventManager
{
public:

    /**
     * @brief   インスタンス取得
     */
    static EventManager& Instance();
    
    ~EventManager() noexcept;

    /**
    *  @brief コピー禁止
    */
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    /**
     * @brief   イベントが登録されていないかチェック
     * @retval  イベントが登録されていなければtrue
     * @note    クラス名が違っても、文字列のハッシュ値が同じだと同じクラスだと判定される。そのときはあきらめて他の文字列を使うこと。 
     */
    bool IsValidEvent( const EventType& type ) const;

    /**
     * @brief   イベントを登録
     */
    bool RegisterEvent( const EventType& type );
    
    /**
     * @brief   ハンドラを登録
     */ 
    bool AddHandler( const EventType& type, const IEventHandlerPtr& handler );

    /**
     * @brief   登録されたハンドラを消す
     *          登録するイベントハンドラには名前を付けてある
     *          引数に渡した名前のハンドラをすべて削除する
     */
    bool RemoveHandler( const HashedString& handler_name );

    /**
     * @brief  イベントをキューに追加
     * @note   スレッドセーフでないので、マルチスレッドでは使わないこと。
     */
    bool QueueEvent( const IEventPtr& event );
    
    /**
     * @brief   その場でハンドラを呼び出す
     */
    bool SendEvent( const IEventPtr& event );
    
    /**
     * @brief   イベントキューの更新
     * @param [in] time_limit   制限時間
     * @param [in] is_ignore_events   true なら制限時間を過ぎたとき、残りのイベントを捨てる
     * @return    true   Queue内のすべてのイベントが正常に処理された
     *            false  Queue内のすべてのイベントがイベントハンドラで処理できなかった
     *                   制限時間TimeOutの場合は次回持ち越しのイベントがある
     */
    bool Update( uint64_t time_limit_ms = sk_UpdateTimeLimitInf,
                 bool is_ignore_events = false );
    
private:

    EventManager();

    static constexpr uint64_t sk_UpdateTimeLimitInf = std::numeric_limits<uint64_t>::max();
    static constexpr int sk_EventQueueNum = 2;   //!  イベントキューの数

    typedef std::vector<EventType> EventTypeVector;
    typedef std::vector<IEventHandlerPtr> EventHandlerVector;
    typedef std::map<EventType, EventHandlerVector> EventHandlerTable;
    typedef std::queue<IEventPtr> EventQueue;

    //! 登録されたイベントリスト
    EventTypeVector   m_RegisteredEvent;
    //! イベントキュー
    EventQueue        m_EventQueue[sk_EventQueueNum];
    //! 現在のイベントキューインデックス
    int               m_CurrentQueueNum;
    //! イベントとハンドラの対応表
    EventHandlerTable m_HandlerMappingTable;
};

}

#endif
