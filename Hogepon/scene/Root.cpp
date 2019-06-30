
#include "scene/Root.hpp"
#include "scene/GameScene.hpp"

namespace scene
{
    Root::Root()
    {
        m_RootScene.add<Title>(U"Root/Title");
        m_RootScene.add<MainMenu>(U"Root/MainMenu");
        m_RootScene.add<GameModeRoot>(U"Root/GameModeRoot");
    }

    void Root::Update()
    {
        m_RootScene.updateScene();
    }

    void Root::Draw()
    {
        m_RootScene.drawScene();
    }



    Title::Title(const TitleScene::Scene::InitData& init)
        : 
        IScene(init),
        m_Timer(true),
        m_BlinkWelcomeText(2000)
    {
    }

    void Title::update()
    {
        if (s3d::KeyEnter.pressed()) {
            changeScene(U"Root/MainMenu");
        }
    }

    void Title::draw() const
    {
        s3d::ColorF color(U"#0F9");
        color.a = m_BlinkWelcomeText.Out(m_Timer.ms64());

        s3d::FontAsset(U"TitleFont")(U"PANEL DE PON").drawAt(s3d::Window::Center().x, s3d::Window::Height() * 0.3, s3d::Color(U"#0F9"));
        s3d::FontAsset(U"TitlePressAnyKey")(U"Press Enter Key.").drawAt(s3d::Window::Center().x, s3d::Window::Height() * 0.6, color);
    }



    MainMenu::MainMenu(const TitleScene::Scene::InitData& init)
        : 
        IScene(init),
        m_MenuTextList(),
        m_SelectedCursorRect(),
        m_CursorIndex(0)
    {
        m_MenuTextList.push_back(s3d::FontAsset(U"MainMenuFont")(U"Endless"));
        m_MenuTextList.push_back(s3d::FontAsset(U"MainMenuFont")(U"Score Attack"));
        m_MenuTextList.push_back(s3d::FontAsset(U"MainMenuFont")(U"2P V.S"));

        m_SelectedCursorRect = calculateCursorSize();
    }


    void MainMenu::update()
    {
        if (s3d::KeyUp.down()) {
            --m_CursorIndex;
            if (m_CursorIndex < 0) {
                m_CursorIndex = std::max(static_cast<int>(m_MenuTextList.size()) - 1, 0);
            }
        }
        if (s3d::KeyDown.down()) {
            ++m_CursorIndex;
            if (m_CursorIndex >= m_MenuTextList.size()) {
                m_CursorIndex = 0;
            }
        }
        
        if (s3d::KeyEnter.down()) {
            if (static_cast<GameMenu>(m_CursorIndex) == ScoreAttack) {
                changeScene(U"Root/GameModeRoot");
            }
        }
    }

    void MainMenu::draw() const
    {
        double x = s3d::Window::Width() * 0.3;
        double y = s3d::Window::Height() * 0.2;
        double menu_pitch_y = m_SelectedCursorRect.h + 40;

        drawMenu(x, y, menu_pitch_y);
        drawCursor(x, y, menu_pitch_y);
    }

    s3d::RectF MainMenu::calculateCursorSize()
    {
        RectF biggest_rect(0.0, 0.0);
        for (auto menu : m_MenuTextList) {
            RectF rect = menu.region();
            if (rect.w > biggest_rect.w) {
                biggest_rect.w = rect.w;
            }
            if (rect.h > biggest_rect.h) {
                biggest_rect.h = rect.h;
            }
        }

        return biggest_rect;
    }

    void MainMenu::drawMenu( double x, double y, double menu_pitch_y ) const
    {
        int menu_cnt = 0;
        for (auto menu : m_MenuTextList) {
            menu.drawAt(x, y + menu_pitch_y * menu_cnt, s3d::Color(U"#0F9"));
            ++menu_cnt;
        }
    }

    void MainMenu::drawCursor(double x, double y, double menu_pitch_y) const
    {
        double dx = (m_SelectedCursorRect.w / 2);
        double dy = (m_SelectedCursorRect.h / 2);
        m_SelectedCursorRect.stretched(15.0, 10.0).shearedX(15.0).movedBy(x - dx, y + menu_pitch_y * m_CursorIndex - dy).drawFrame(2.0, s3d::Color(U"#0F9"));
    }


    GameModeRoot::GameModeRoot(const TitleScene::Scene::InitData& init)
        :
        IScene(init)
    {
        m_GameModeRoot.add<ScoreAttack>(U"Root/GameModeRoot/ScoreAttack");
    }

    void GameModeRoot::update()
    {
        m_GameModeRoot.updateScene();
    }

    void GameModeRoot::draw() const
    {
        m_GameModeRoot.drawScene();
    }
}

