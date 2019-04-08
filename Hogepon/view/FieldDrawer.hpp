#pragma once

#include "util/SubTextures.hpp"
#include "core/GameLogic.hpp"
#include "view/PanelDrawer2D.hpp"

class FieldDrawer
{
public:

	FieldDrawer();
	~FieldDrawer() noexcept;

	void Draw(const GameLogic& gamelogic);

private:
	
	void drawCursor(const GameLogic& gamelogic);

	PanelDrawer2D m_PanelDrawer2D;
    SubTextures   m_CursorTexture;
    s3d::Texture  m_FrameTexture;
};