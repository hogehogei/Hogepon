#pragma once

#include <Siv3D.hpp>
#include "util/SubTextures.hpp"
#include "core/GameLogic.hpp"
#include "view/PanelDrawer2D.hpp"
#include "view/effect/easing/Shake.h"
#include "event/Event.hpp"

class FieldDrawer
{
public:

	FieldDrawer( const s3d::String& filepath );
	~FieldDrawer() noexcept;

    void Update();
	void Draw(const GameLogic& gamelogic) const;
    PanelDrawer2D::DrawSetting DrawSetting() const;
    int Width(const GameLogic& gamelogic) const;
    int Height(const GameLogic& gamelogic) const;

    static bool EventHandler(FieldDrawer* self, const exlib::IEvent& event);

private:
	
    int getPosLowerLeftX() const;
    int getPosLowerLeftY() const;
    void updateShakeAmptitude();

    void readDrawSetting(const s3d::String& filepath);
	void drawCursor(const GameLogic& gamelogic) const;
    void startShakeField();

	PanelDrawer2D m_PanelDrawer2D;
    SubTextures   m_CursorTexture;

    int m_PanelSize;
    int m_LowerLeft_X;
    int m_LowerLeft_Y;

    s3d::Timer m_Timer;
    easing::Shake m_EasingShake;
    int m_ShakeAmptitude_Y;
};