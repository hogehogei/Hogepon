
# include <Siv3D.hpp> // OpenSiv3D v0.3.1
#include "core/GameLogic.hpp"
#include "view/FieldDrawer.hpp"

void Main()
{
	//Graphics::SetBackground(ColorF(0.8, 0.9, 1.0));
	PlayerID player = 1;
	GameLogic gamelogic(player, GAMEMODE_1P_ENDLESS, U"setting/leveldata.xml");
	FieldDrawer drawer;

	while (System::Update())
	{
            gamelogic.Update();
            drawer.Draw(gamelogic);
	}
}
