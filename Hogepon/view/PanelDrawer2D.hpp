#pragma once

#include <Siv3D.hpp>
#include "core/GameLogic.hpp"
#include "core/Panel.hpp"
#include "util/SubTextures.hpp"
#include "util/HashedString.hpp"
#include "util/IEvent.hpp"
#include "event/Event.hpp"
#include "view/effect/PanelDeleteEffect.hpp"

class PanelDrawer2D
{
public:
	struct DrawSetting
	{
		int BaseX;
		int BaseY;
		int PanelSize;

		PanelDrawer2D::DrawSetting()
			: BaseX(0), BaseY(0), PanelSize(0) {}

		PanelDrawer2D::DrawSetting(int x, int y, int panel_unit)
			: BaseX(x), BaseY(y), PanelSize(panel_unit) {}
	};

public:

	PanelDrawer2D();
	~PanelDrawer2D() noexcept;

	void SetDrawSetting(const PanelDrawer2D::DrawSetting& draw_setting);
    void Update();
	void DrawPanels(const GameLogic& gamelogic) const;
    void DrawEffects(const GameLogic& gamelogic) const;
    
    static bool EventHandler(PanelDrawer2D* self, const exlib::IEvent& event);

private:

    using PanelDeleteEffectPtr = std::shared_ptr<PanelDeleteEffect>;

	void drawPanels(const GameLogic& gamelogic, int x, int y) const;
    void drawOjyamaBlockBack(const GameLogic& gamelogic, int x, int y) const;
    void drawOjyamaPanelExpression(const GameLogic& gamelogic, int x, int y) const;

    void drawPanelMark(const GameLogic& gamelogic, const Panel& panel, int draw_pixel_x, int draw_pixel_y, const s3d::Color& diffuse) const;
    void drawPanelBeforeDelete(const GameLogic& gamelogic, const Panel& panel, int draw_pixel_x, int draw_pixel_y) const;
    void drawPanelSurprisedFace(const GameLogic& gamelogic, const Panel& panel, int draw_pixel_x, int draw_pixel_y) const;
    void drawOjyamaFace(const GameLogic& gamelogic) const;
    void drawUncompressPanel(const s3d::TextureRegion& ojyama_texture, int draw_pixel_x, int draw_pixel_y) const;
    s3d::TextureRegion getOjyamaSubTexture(const GameLogic& gamelogic, int x, int y) const;

	int calculateDrawPos_X(const GameLogic& gamelogic, const Panel& panel, int x) const;
	int calculateDrawPos_Y(const GameLogic& gamelogic, const Panel& panel, int y) const;
    void createPanelDeleteEffectTask(const PanelDeletedEvent& event);
    void eraseEffectsLifeTimeExceeded();

	PanelDrawer2D::DrawSetting m_DrawSetting;
    SubTextures m_PanelTexture;
    SubTextures m_OjyamaTexture;
    std::list<PanelDeleteEffectPtr> m_Effects;
};
