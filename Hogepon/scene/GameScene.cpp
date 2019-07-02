
#include "scene/GameScene.hpp"

namespace scene
{
    ScoreAttack::ScoreAttack(const GameModeScene::Scene::InitData& init)
        :
        IScene(init),
        m_ReadyTimer(5.0, true),
        m_RemainTimer(60.0*2.0, false),
        m_Gamelogic(1, GAMEMODE_1P_ENDLESS, U"setting/leveldata.xml"),
        m_Drawer(U"setting/fielddraw_1P.xml")
    {}

    void ScoreAttack::update()
    {
        if (m_ReadyTimer.reachedZero()){
            if (!m_RemainTimer.isRunning()) {
                m_RemainTimer.start();
            }

            if (!m_RemainTimer.reachedZero()) {
                m_Gamelogic.Update();
                m_Drawer.Update();
            }
        }
    }

    void ScoreAttack::drawGameStartReady() const
    {
        PanelDrawer2D::DrawSetting setting = m_Drawer.DrawSetting();
        double dx = m_Drawer.Width(m_Gamelogic) * 0.5;
        double dy = m_Drawer.Height(m_Gamelogic) * 0.7;

        if (m_ReadyTimer.remaining() >= s3d::Milliseconds(3000)) {
            s3d::FontAsset(U"GameStartReady")(U"Ready.").drawAt(setting.BaseX + dx, setting.BaseY - dy, s3d::ColorF(U"#FF9100"));
        }
        else {
            int countdown = (m_ReadyTimer.remaining().count() / 1000) + 1;
            s3d::FontAsset(U"GameStartReady")(s3d::Format(countdown)).drawAt(setting.BaseX + dx, setting.BaseY - dy, s3d::ColorF(U"#FF9100"));
        }
    }

    void ScoreAttack::drawScore() const
    {
        s3d::FontAsset(U"Score")(U"Score").draw(400, 100, s3d::Color(0xFF, 0xFF, 0xFF));
        s3d::FontAsset(U"Score")(s3d::Format(m_Gamelogic.GetGameState().Score())).draw(400, 140, s3d::Color(0xFF, 0xFF, 0xFF));
    }

    void ScoreAttack::drawRemainTime() const
    {
        uint64_t remain_time = (m_RemainTimer.remaining().count() / 1000);
        uint64_t remain_minutes = remain_time / 60;
        uint64_t remain_seconds = remain_time % 60;

        s3d::FontAsset(U"ScoreAttackRemainTime")(U"Time").draw(400, 200, s3d::Color(0xFF, 0xFF, 0xFF));
        s3d::FontAsset(U"ScoreAttackRemainTime")(s3d::Format(remain_minutes, U":", s3d::Pad(remain_seconds, { 2, L'0' }))).draw(400, 240, s3d::Color(0xFF, 0xFF, 0xFF));
    }

    void ScoreAttack::draw() const
    {
        m_Drawer.Draw(m_Gamelogic);
        drawScore();
        drawRemainTime();

        if (!m_ReadyTimer.reachedZero()) {
            drawGameStartReady();
        }
    }
}
