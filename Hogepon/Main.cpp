
# include <Siv3D.hpp> // OpenSiv3D v0.3.1
#include "core/GameLogic.hpp"
#include "initialize/Initialize.hpp"
#include "view/FieldDrawer.hpp"
#include "util/EventManager.hpp"
#include "event/Event.hpp"
#include "scene/Root.hpp"

void Main()
{
    RegisterAllEvent();
    RegisterFontAssets();

    std::unique_ptr<scene::Root> root_scene = std::make_unique<scene::Root>();

	while (System::Update())
	{
        root_scene->Update();
        exlib::EventManager& event_manager = exlib::EventManager::Instance();
        event_manager.Update();
        root_scene->Draw();
	}
}
