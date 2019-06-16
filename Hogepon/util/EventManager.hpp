
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
     * @brief   �C���X�^���X�擾
     */
    static EventManager& Instance();
    
    ~EventManager() noexcept;

    /**
    *  @brief �R�s�[�֎~
    */
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    /**
     * @brief   �C�x���g���o�^����Ă��Ȃ����`�F�b�N
     * @retval  �C�x���g���o�^����Ă��Ȃ����true
     * @note    �N���X��������Ă��A������̃n�b�V���l���������Ɠ����N���X���Ɣ��肳���B���̂Ƃ��͂�����߂đ��̕�������g�����ƁB 
     */
    bool IsValidEvent( const EventType& type ) const;

    /**
     * @brief   �C�x���g��o�^
     */
    bool RegisterEvent( const EventType& type );
    
    /**
     * @brief   �n���h����o�^
     */ 
    bool AddHandler( const EventType& type, const IEventHandlerPtr& handler );

    /**
     * @brief   �o�^���ꂽ�n���h��������
     *          �o�^����C�x���g�n���h���ɂ͖��O��t���Ă���
     *          �����ɓn�������O�̃n���h�������ׂč폜����
     */
    bool RemoveHandler( const HashedString& handler_name );

    /**
     * @brief  �C�x���g���L���[�ɒǉ�
     * @note   �X���b�h�Z�[�t�łȂ��̂ŁA�}���`�X���b�h�ł͎g��Ȃ����ƁB
     */
    bool QueueEvent( const IEventPtr& event );
    
    /**
     * @brief   ���̏�Ńn���h�����Ăяo��
     */
    bool SendEvent( const IEventPtr& event );
    
    /**
     * @brief   �C�x���g�L���[�̍X�V
     * @param [in] time_limit   ��������
     * @param [in] is_ignore_events   true �Ȃ琧�����Ԃ��߂����Ƃ��A�c��̃C�x���g���̂Ă�
     * @return    true   Queue���̂��ׂẴC�x���g������ɏ������ꂽ
     *            false  Queue���̂��ׂẴC�x���g���C�x���g�n���h���ŏ����ł��Ȃ�����
     *                   ��������TimeOut�̏ꍇ�͎��񎝂��z���̃C�x���g������
     */
    bool Update( uint64_t time_limit_ms = sk_UpdateTimeLimitInf,
                 bool is_ignore_events = false );
    
private:

    EventManager();

    static constexpr uint64_t sk_UpdateTimeLimitInf = std::numeric_limits<uint64_t>::max();
    static constexpr int sk_EventQueueNum = 2;   //!  �C�x���g�L���[�̐�

    typedef std::vector<EventType> EventTypeVector;
    typedef std::vector<IEventHandlerPtr> EventHandlerVector;
    typedef std::map<EventType, EventHandlerVector> EventHandlerTable;
    typedef std::queue<IEventPtr> EventQueue;

    //! �o�^���ꂽ�C�x���g���X�g
    EventTypeVector   m_RegisteredEvent;
    //! �C�x���g�L���[
    EventQueue        m_EventQueue[sk_EventQueueNum];
    //! ���݂̃C�x���g�L���[�C���f�b�N�X
    int               m_CurrentQueueNum;
    //! �C�x���g�ƃn���h���̑Ή��\
    EventHandlerTable m_HandlerMappingTable;
};

}

#endif
