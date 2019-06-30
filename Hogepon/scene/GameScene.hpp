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

        s3d::Timer m_ReadyTimer;
        GameLogic m_Gamelogic;
        FieldDrawer m_Drawer;
    };

}