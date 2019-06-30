#pragma once

#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include "view/effect/easing/RainbowCurve.hpp"

namespace scene
{
    using TitleScene = s3d::SceneManager<s3d::String>;

    class Root
    {
    public:

        Root();
        void Update();
        void Draw();

    private:

        s3d::SceneManager<s3d::String> m_RootScene;
    };


    class Title : public TitleScene::Scene
    {
    public:
        
        Title(const TitleScene::Scene::InitData& init);
        void update() override;
        void draw() const override;

    private:

        s3d::Stopwatch m_Timer;
        easing::RainbowCurve m_BlinkWelcomeText;
    };

    class MainMenu : public TitleScene::Scene
    {
        using TextList = std::list<s3d::DrawableText>;

        enum GameMenu
        {
            Endless = 0,
            ScoreAttack = 1,
            Versus_2P = 2,
        };

    public:

        MainMenu(const TitleScene::Scene::InitData& init);
        void update() override;
        void draw() const override;

    private:

        s3d::RectF calculateCursorSize();
        void drawMenu(double x, double y, double menu_pitch_y) const;
        void drawCursor(double x, double y, double menu_pitch_y) const;

        TextList m_MenuTextList;
        s3d::RectF m_SelectedCursorRect;
        int m_CursorIndex;
    };

    class GameModeRoot : public TitleScene::Scene
    {
    public:

        GameModeRoot(const TitleScene::Scene::InitData& init);
        void update() override;
        void draw() const override;

    private:

        s3d::SceneManager<s3d::String> m_GameModeRoot;
    };
}