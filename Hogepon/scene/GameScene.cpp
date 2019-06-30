
#include "scene/GameScene.hpp"

namespace scene
{
    ScoreAttack::ScoreAttack(const GameModeScene::Scene::InitData& init)
        :
        IScene(init),
        m_ReadyTimer(5.0, true),
        m_Gamelogic(1, GAMEMODE_1P_ENDLESS, U"setting/leveldata.xml"),
        m_Drawer(U"setting/fielddraw_1P.xml")
    {}

    void ScoreAttack::update()
    {
        if (m_ReadyTimer.reachedZero()){
            m_Gamelogic.Update();
            m_Drawer.Update();
        }
    }

    void ScoreAttack::draw() const
    {
        m_Drawer.Draw(m_Gamelogic);

        if (!m_ReadyTimer.reachedZero()) {
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
    }
}
