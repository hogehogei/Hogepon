
#ifndef   PANEPON_GAME_FIELD_STATE_HPP
#define   PANEPON_GAME_FIELD_STATE_HPP

#include <stdint.h>
#include "core/Panel.hpp"
#include "GameFieldSetting.hpp"

class GameState {
public:
    

    static constexpr uint32_t STATE_NORMAL              = (1 << 0); // ゲーム中、普通の状態
    static constexpr uint32_t STATE_AUTO_SERIAGARI      = (1 << 1); // 自動せり上がり有効
    static constexpr uint32_t STATE_SERIAGARI           = (1 << 2); // せり上がりの状態
    static constexpr uint32_t STATE_WARNING             = (1 << 3); // ゲーム中、ピンチ！
    static constexpr uint32_t STATE_DANGER              = (1 << 4);
    static constexpr uint32_t STATE_DELETE_PANEL        = (1 << 5);
    static constexpr uint32_t STATE_UNCOMPRESS_OJYAMA   = (1 << 6);
    static constexpr uint32_t STATE_GAME_OVER           = (1 << 7);
        
public:

    GameState();
    ~GameState() noexcept = default;

    int Score() const { return m_Score; }
    int ChainCount() const { return m_ChainCount; }

	// せり上がりカウンタ操作
	void IncSeriagariCount(int count);
	void SetSeriagariCount(int count);
	int SeriagariCount() const;

	// せり上がり開始待ちタイマー
	void SetSeriagariWaitTimer(int time);
	int SeriagariWaitTimer() const;
	void DecSeriagariWaitTimer();

	// ゲームオーバータイマー
	void SetGameOverWaitTimer(int time);
	int GameOverWaitTimer() const;
	void DecGameOverWaitTimer();
    
    void SetState( unsigned state );
    void ClearState( unsigned state );
    
	bool IsGameOver() const;
	bool IsDanger() const;
	bool IsSeriagari() const;
	bool IsAutoSeriagari() const;
	bool IsDeletePanel() const;
	bool IsUncompressOjyama() const;

    void PanelDeleted(const Panel& panel, int xpos, int ypos);
	void Doujikeshi( const GameFieldSetting& setting, int doujikeshi_count);
	void IncChainCount( const GameFieldSetting& setting );
	void EndChain();
    void FallOjyama();

private:
        
    int m_Score;
    int m_ChainCount;
	int m_SeriagariCount;

    int m_SeriagariWaitTimer;
    int m_GameOverWaitTimer;

	bool m_Is_InChain;

    uint32_t m_FieldState;
};

#endif   // PANEPON_GAME_FIELD_STATE_HPP


