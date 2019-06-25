
#pragma once

#include <Siv3D.hpp>
#include "core/Score.hpp"

class GameFieldSetting
{
public:

    explicit GameFieldSetting( const s3d::String& filepath );
    ~GameFieldSetting() noexcept = default;

    s3d::String levelName() const { return m_ChallengeLevel; }
	int Width() const { return m_FieldWidth; }
    int Height() const { return m_FieldHeight; }

	// せり上がり関連
	int IncFastSeriagari() const { return m_IncFastSeriagari; }
	int IncAutoSeriagari() const { return m_IncAutoSeriagari; }
	int SeriagariCountMax() const { return m_SeriagariCountMax; }

	// ゲームオーバー関連
	int GameOverTimeLimit() const { return m_GameOverTimeLimit; }

	// パネル消去関連
	int DecPanelDeleteBeforeWait() const { return m_DecPanelDeleteBeforeWait; }
	int DeleteBeforeWait() const { return m_DeleteBeforeWait; }
	int WaitTimePanelDel() const { return m_WaitTimePanelDel; }
	int DecPanelDeleteWait() const { return m_DecPanelDeleteWait; }
	int DecPanelDeleteAfterWait() const { return m_DecPanelDeleteAfterWait; }

	// パネル落下関連
	int IncFallBeforeWait() const { return m_IncFallBeforeWait; }
	int FallBeforeWaitMax() const { return m_FallBeforeWaitMax; }
	int IncFallCount() const { return m_IncFallCount; }
	int FallCountMax() const { return m_FallCountMax; }
	int IncFallAfterWait() const { return m_IncFallAfterWait; }
	int FallAfterWaitMax() const { return m_FallAfterWaitMax; }

	// パネル交換関連
	int IncSwappingCount() const { return m_IncSwappingCount; }
	int SwappingCountMax() const { return m_SwappingCountMax; }

    // お邪魔パネル解凍関連
    int DecOjyamaPanelDeleteBeforeWait() const { return m_DecOjyamaPanelDeleteBeforeWait; }
    int DecOjyamaPanelDeleteWait() const { return m_DecOjyamaPanelDeleteWait; }
    int DecOjyamaPanelDeleteAfterWait() const { return m_DecOjyamaPanelDeleteAfterWait; }
    int UncompressBeforeWait() const { return m_UncompressBeforeWait; }

    // 得点関連
    int DoujikeshiScore(int doujikeshi_count) const;
    int ChainScore(int chain_count) const;
    
private:
    
	void parseGameFieldSettings(const s3d::XMLElement& elem);
	void parseFieldInfo(const s3d::XMLElement& elem);

    s3d::String m_ChallengeLevel;

	int m_IncFastSeriagari;
	int m_IncAutoSeriagari;
	int m_SeriagariCountMax;

	int m_GameOverTimeLimit;

    int m_DecPanelDeleteBeforeWait;
    int m_DeleteBeforeWait;
	int m_WaitTimePanelDel;
	int m_DecPanelDeleteWait;
	int m_DecPanelDeleteAfterWait;

	int m_IncFallBeforeWait;
	int m_FallBeforeWaitMax;
	int m_IncFallCount;
	int m_FallCountMax;
	int m_IncFallAfterWait;
	int m_FallAfterWaitMax;

	int m_IncSwappingCount;
	int m_SwappingCountMax;

    int m_DecOjyamaPanelDeleteBeforeWait;
    int m_DecOjyamaPanelDeleteWait;
    int m_DecOjyamaPanelDeleteAfterWait;
    int m_UncompressBeforeWait;

	int m_FieldWidth;
	int m_FieldHeight;

    ::DoujikeshiScore m_DoujikeshiScore;
    ::ChainScore      m_ChainScore;
};


