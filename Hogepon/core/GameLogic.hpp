#ifndef   PANEPON_GAME_FIELD_LOGIC_HPP
#define   PANEPON_GAME_FIELD_LOGIC_HPP

#include <Siv3D.hpp>
#include "core/Panel.hpp"
#include "core/GameFieldSetting.hpp"
#include "core/GameFieldState.hpp"
#include "core/PanelContainer.hpp"

struct CursorPos
{
	int cx;
	int cy;
};

using PlayerID = uint32;
enum GameMode
{
	GAMEMODE_1P_ENDLESS = 0,
};

class GameLogic
{
   
public:

    GameLogic( PlayerID id,
               GameMode mode,
               const s3d::String& setting_filepath );
    GameLogic(const GameLogic&) = delete;
    GameLogic& operator=(const GameLogic&) = delete;
    ~GameLogic() noexcept = default;

    const PanelContainer& GetPanelContainer() const { return m_PanelContainer; }
	const CursorPos& GetCursorPos() const { return m_Cursor; }
    const GameState& GetGameState() const { return m_State; }
    const GameFieldSetting& GetFieldSetting() const { return m_FieldSetting; }
    GameMode GetGameMode() const { return m_GameMode; }
    PlayerID GetPlayerID() const { return m_PlayerID; }

    void Update();
    
private:
   
    // パネルの状態を更新します
    void updatePanel( int x, int y );
    // お邪魔パネルのアップデートします
    void updateOjyamaPanel( int x, int y );

	void clearPanelMark();
	void processInput();
	bool swapPanel();
	void check_IsPlayerPinch();
    void fallOjyamaLine();
	bool canSeriagari();
	void doSeriagari();
	void judgeGameOver();
	void deletePanel();
	void changeState_PanelsDeleting();

    void uncompressOjyama();
    void markUncompressOjyama(int x, int y);
    void setUncompressOjyamaTimer();
	int countDeleteMarkPanel();
    int countDeleteMarkUncompress();
	
	void update_PanelDefault(int x, int y, Panel& panel);
	void update_PanelSwapping(int x, int y, Panel& panel);
	void update_PanelFallBeforeWait(int x, int y, Panel& panel);
	void update_PanelFalling(int x, int y, Panel& panel);
	void update_PanelFallAfterWait(int x, int y, Panel& panel);
	void update_PanelDeleteBeforeWait(Panel& panel);
	void update_PanelDelete(int x, int y, Panel& panel);
	void update_PanelDeleteAfterWait(int x, int y, Panel& panel);

    void update_OjyamaDefault(int x, int y);
    void update_OjyamaFallBeforeWait(int x, int y);
    void update_OjyamaFalling(int x, int y);
    void update_OjyamaFallAfterWait(int x, int y);
    void update_OjyamaUncompressBeforeWait(int x, int y);
    void update_OjyamaUncompress(int x, int y);
    void update_OjyamaUncompressAfterWait(int x, int y);


    /**
    * @brief お邪魔パネルのコリジョンチェックします
    * @return 下で引っかかったパネル、もしくはスペースパネル
    */
	void setChainFlag(const PanelPos& pos);

    const PlayerID	m_PlayerID;
    const GameMode	m_GameMode;
    
	PanelContainer	  m_PanelContainer;			                // パネルコンテナ
	CursorPos		  m_Cursor;				                	// カーソル位置
    GameFieldSetting  m_FieldSetting;		                	// フィールドの基本情報
    GameState         m_State;				                	// 状態を表す構造体
};


#endif   // PANEPON_GAME_FIELD_LOGIC_HPP

