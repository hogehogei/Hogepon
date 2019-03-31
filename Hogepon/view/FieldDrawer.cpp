
#include <Siv3D.hpp>
#include "view/FieldDrawer.hpp"
#include "view/PanelDrawer2D.hpp"

FieldDrawer::FieldDrawer()
	: m_PanelDrawer2D()
{
	m_PanelDrawer2D.SetDrawSetting(PanelDrawer2D::DrawSetting(50, 400, 40));
}

FieldDrawer::~FieldDrawer()
{}

void FieldDrawer::Draw(const GameLogic& gamelogic)
{
	m_PanelDrawer2D.DrawPanels(gamelogic);
	drawCursor(gamelogic);
}

void FieldDrawer::drawCursor(const GameLogic& gamelogic)
{
	int cursor_x = gamelogic.GetCursorPos().cx;
	int cursor_y = gamelogic.GetCursorPos().cy;
	int panel_unit = 40;
	int draw_x = 50 + cursor_x * panel_unit;
	int draw_y = 400 - cursor_y * panel_unit;
}