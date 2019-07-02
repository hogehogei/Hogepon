
#include "initialize/Initialize.hpp"

#include <Siv3D.hpp>
#include "util/EventManager.hpp"
#include "event/Event.hpp"


void RegisterAllEvent()
{
    exlib::EventManager& manager = exlib::EventManager::Instance();

    // 登録したいイベントを羅列する
    manager.RegisterEvent(OjyamaFallEvent::sk_EventType);
    manager.RegisterEvent(PanelDeletedEvent::sk_EventType);
    manager.RegisterEvent(OjyamaAttackEvent::sk_EventType);
}

void RegisterFontAssets()
{
    // タイトル画面
    FontAsset::Register(U"TitleFont", 60, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");
    FontAsset::Register(U"TitlePressAnyKey", 20, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");

    // メインメニュー
    FontAsset::Register(U"MainMenuFont", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");

    // ゲーム開始レディ
    FontAsset::Register(U"GameStartReady", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");

    // スコアアタック
    FontAsset::Register(U"Score", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");
    FontAsset::Register(U"ScoreAttackRemainTime", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");
}
