#pragma once

#include <Siv3D.hpp>
#include <HamFramework.hpp>

#include "core/GameLogic.hpp"
#include "view/FieldDrawer.hpp"

namespace scene
{
    using GameModeScene = s3d::SceneManager<s3d::String>;

    class ScoreAttack : public GameModeScene::Scene
    {
    public:

        ScoreAttack(const GameModeScene::Scene::InitData& init);
        void update() override;
        void draw() const override;

    private:

        void drawGameStartReady() const;
        void drawScore() const;
        void drawRemainTime() const;

        s3d::Timer m_ReadyTimer;
        s3d::Timer m_RemainTimer;
        GameLogic m_Gamelogic;
        FieldDrawer m_Drawer;
    };

}