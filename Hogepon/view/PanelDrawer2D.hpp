#pragma once

#include <Siv3D.hpp>
#include "core/GameLogic.hpp"
#include "core/Panel.hpp"
#include "util/SubTextures.hpp"

class PanelDrawer2D
{
public:
	struct DrawSetting
	{
		int BaseX;
		int BaseY;
		int PanelUnit;

		PanelDrawer2D::DrawSetting()
			: BaseX(0), BaseY(0), PanelUnit(0) {}

		PanelDrawer2D::DrawSetting(int x, int y, int panel_unit)
			: BaseX(x), BaseY(y), PanelUnit(panel_unit) {}
	};

public:

	PanelDrawer2D();
	~PanelDrawer2D() noexcept;

	void SetDrawSetting(const PanelDrawer2D::DrawSetting& draw_setting);
	void DrawPanels(const GameLogic& gamelogic);

private:

	void drawPanel(const GameLogic& gamelogic, int x, int y);

	int calculateDrawPos_X(const GameLogic& gamelogic, const Panel& panel, int x);
	int calculateDrawPos_Y(const GameLogic& gamelogic, const Panel& panel, int y);

	PanelDrawer2D::DrawSetting m_DrawSetting;
    SubTextures m_PanelTexture;
};