

#include <cassert>
#include "core/GameFieldSetting.hpp"

GameFieldSetting::GameFieldSetting(const s3d::String& filepath)
	: m_ChallengeLevel(),
	m_IncFastSeriagari(0),
	m_IncAutoSeriagari(0),
	m_SeriagariCountMax(0),
	m_GameOverTimeLimit(0),
	m_DecPanelDeleteBeforeWait(0),
	m_DeleteBeforeWait(0),
	m_WaitTimePanelDel(0),
	m_DecPanelDeleteWait(0),
	m_DecPanelDeleteAfterWait(0),
	m_IncFallBeforeWait(0),
	m_FallBeforeWaitMax(0),
	m_IncFallCount(0),
	m_FallCountMax(0),
	m_IncFallAfterWait(0),
	m_FallAfterWaitMax(0),
	m_IncSwappingCount(0),
	m_SwappingCountMax(0),
    m_DecOjyamaPanelDeleteBeforeWait(0),
    m_DecOjyamaPanelDeleteWait(0),
    m_DecOjyamaPanelDeleteAfterWait(0),
    m_UncompressBeforeWait(0),
	m_FieldWidth(0),
	m_FieldHeight(0),
    m_DoujikeshiScore(U"setting/score.xml"),
    m_ChainScore(U"setting/score.xml")

{
	s3d::XMLReader xml(filepath);

	for (auto book = xml.firstChild(); book; book = book.nextSibling()) {
		if (book.name() == s3d::String(U"GameFieldSettings")) {
			parseGameFieldSettings(book);
		}
		if (book.name() == s3d::String(U"FieldInfo")) {
			parseFieldInfo(book);
		}
	}
}

void GameFieldSetting::parseGameFieldSettings(const s3d::XMLElement& elem)
{
	m_ChallengeLevel		   = elem.attribute(U"ChallengeLevel").value_or(U"easy");

	// せり上がり関連
	m_IncFastSeriagari         = s3d::Parse<int>(elem.attribute(U"IncFastSeriagari").value_or(U"0"));
	m_IncAutoSeriagari         = s3d::Parse<int>(elem.attribute(U"IncAutoSeriagari").value_or(U"0"));
	m_SeriagariCountMax        = s3d::Parse<int>(elem.attribute(U"SeriagariCountMax").value_or(U"0"));

	// ゲームオーバー関連
	m_GameOverTimeLimit        = s3d::Parse<int>(elem.attribute(U"GameOverTimeLimit").value_or(U"0"));

	// パネル消去関連
	m_DecPanelDeleteBeforeWait = s3d::Parse<int>(elem.attribute(U"DecPanelDeleteBeforeWait").value_or(U"0"));
	m_DeleteBeforeWait         = s3d::Parse<int>(elem.attribute(U"DeleteBeforeWait").value_or(U"0"));
	m_WaitTimePanelDel         = s3d::Parse<int>(elem.attribute(U"WaitTimePanelDel").value_or(U"0"));
	m_DecPanelDeleteWait       = s3d::Parse<int>(elem.attribute(U"DecPanelDeleteWait").value_or(U"0"));
	m_DecPanelDeleteAfterWait  = s3d::Parse<int>(elem.attribute(U"DecPanelDeleteAfterWait").value_or(U"0"));

	// パネル落下関連
	m_IncFallBeforeWait        = s3d::Parse<int>(elem.attribute(U"IncFallBeforeWait").value_or(U"0"));
	m_FallBeforeWaitMax        = s3d::Parse<int>(elem.attribute(U"FallBeforeWaitMax").value_or(U"0"));
	m_IncFallCount             = s3d::Parse<int>(elem.attribute(U"IncFallCount").value_or(U"0"));
	m_FallCountMax             = s3d::Parse<int>(elem.attribute(U"FallCountMax").value_or(U"0"));
	m_IncFallAfterWait         = s3d::Parse<int>(elem.attribute(U"IncFallAfterWait").value_or(U"0"));
	m_FallAfterWaitMax         = s3d::Parse<int>(elem.attribute(U"FallAfterWaitMax").value_or(U"0"));

	// パネル交換関連
	m_IncSwappingCount         = s3d::Parse<int>(elem.attribute(U"IncSwappingCount").value_or(U"0"));
	m_SwappingCountMax         = s3d::Parse<int>(elem.attribute(U"SwappingCountMax").value_or(U"0"));

    // お邪魔パネル解凍関連
    m_DecOjyamaPanelDeleteBeforeWait = s3d::Parse<int>(elem.attribute(U"DecOjyamaPanelDeleteBeforeWait").value_or(U"0"));
    m_DecOjyamaPanelDeleteWait       = s3d::Parse<int>(elem.attribute(U"DecOjyamaPanelDeleteWait").value_or(U"0"));
    m_DecOjyamaPanelDeleteAfterWait  = s3d::Parse<int>(elem.attribute(U"DecOjyamaPanelDeleteAfterWait").value_or(U"0"));
    m_UncompressBeforeWait           = s3d::Parse<int>(elem.attribute(U"UncompressBeforeWait").value_or(U"0"));
}

void GameFieldSetting::parseFieldInfo(const s3d::XMLElement& elem)
{
	m_FieldWidth  = s3d::Parse<int>(elem.attribute(U"FieldWidth").value_or(U"0"));
	m_FieldHeight = s3d::Parse<int>(elem.attribute(U"FieldHeight").value_or(U"0"));
}

int GameFieldSetting::DoujikeshiScore(int doujikeshi_count) const
{
    return m_DoujikeshiScore.Score(doujikeshi_count);
}

int GameFieldSetting::ChainScore(int chain_count) const
{
    return m_ChainScore.Score(chain_count);
}
