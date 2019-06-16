#pragma once

#include <Siv3D.hpp>
#include "util/SubTextures.hpp"
#include "util/EasingShake.h"
#include "core/GameLogic.hpp"
#include "view/PanelDrawer2D.hpp"

class FieldDrawer
{
public:

	FieldDrawer();
	~FieldDrawer() noexcept;

	void Draw(const GameLogic& gamelogic);
    void StartShakeField();

    static bool EventHandler(FieldDrawer* self, const exlib::IEvent& event);

private:
	
    int getPosLowerLeftX() const;
    int getPosLowerLeftY() const;
    void updateShakeAmptitude();

    void readDrawSetting(const s3d::String& filepath);
	void drawCursor(const GameLogic& gamelogic);

	PanelDrawer2D m_PanelDrawer2D;
    SubTextures   m_CursorTexture;
    s3d::Texture  m_FrameTexture;

    int m_PanelSize;
    int m_LowerLeft_X;
    int m_LowerLeft_Y;

    EasingShake m_EasingShake;
    int m_ShakeAmptitude_Y;
};