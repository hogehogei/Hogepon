
#include "initialize/Initialize.hpp"

#include <Siv3D.hpp>
#include "util/EventManager.hpp"
#include "event/Event.hpp"


void RegisterAllEvent()
{
    exlib::EventManager& manager = exlib::EventManager::Instance();

    // �o�^�������C�x���g�𗅗񂷂�
    manager.RegisterEvent(OjyamaFallEvent::sk_EventType);
    manager.RegisterEvent(PanelDeletedEvent::sk_EventType);
    manager.RegisterEvent(OjyamaAttackEvent::sk_EventType);
}

void RegisterFontAssets()
{
    // �^�C�g�����
    FontAsset::Register(U"TitleFont", 60, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");
    FontAsset::Register(U"TitlePressAnyKey", 20, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");

    // ���C�����j���[
    FontAsset::Register(U"MainMenuFont", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");

    // �Q�[���J�n���f�B
    FontAsset::Register(U"GameStartReady", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");

    // �X�R�A�A�^�b�N
    FontAsset::Register(U"Score", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");
    FontAsset::Register(U"ScoreAttackRemainTime", 40, FileSystem::InitialPath() + U"/fonts/x14y20pxScoreDozer.ttf");
}
