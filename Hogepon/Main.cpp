
# include <Siv3D.hpp> // OpenSiv3D v0.3.1
#include "core/GameLogic.hpp"
#include "view/FieldDrawer.hpp"
#include "util/EventManager.hpp"
#include "event/Event.hpp"

void Main()
{
    RegisterAllEvent();

	PlayerID player = 1;
	GameLogic gamelogic(player, GAMEMODE_1P_ENDLESS, U"setting/leveldata.xml");
	FieldDrawer drawer;

	while (System::Update())
	{
        gamelogic.Update();

        exlib::EventManager& event_manager = exlib::EventManager::Instance();
        event_manager.Update();

        drawer.Draw(gamelogic);
	}
}
