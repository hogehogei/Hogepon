
#include <Siv3D.hpp>
#include "view/FieldDrawer.hpp"
#include "view/PanelDrawer2D.hpp"

FieldDrawer::FieldDrawer()
	: m_PanelDrawer2D(),
      m_CursorTexture()
{
	m_PanelDrawer2D.SetDrawSetting(PanelDrawer2D::DrawSetting(50, 400, 40));
    m_CursorTexture.ReadSetting(U"setting/cursor.xml");
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

    // パネルせり上がり中の移動位置を計算
    int seriagari_max = gamelogic.GetFieldSetting().SeriagariCountMax();
    int seriagari_dy = (static_cast<double>(gamelogic.GetGameState().SeriagariCount()) / seriagari_max) * 40;

	int panel_unit = 40;
    int draw_x = 50 + cursor_x * panel_unit - 7;
    int draw_y = 400 - cursor_y * panel_unit - 7 - seriagari_dy;

    m_CursorTexture.SubTexture(U"Cursor").draw(draw_x, draw_y);
}