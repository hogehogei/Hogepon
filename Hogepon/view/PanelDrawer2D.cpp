
#include "view/PanelDrawer2D.hpp"

struct PanelSubTexTableEnt
{
    s3d::String back_identifier;
    s3d::String mark_identifier;
};

// パネルの画像は一枚のテクスチャから一部分を切り出して描画するので、
// 切り出し位置を設定ファイル化している
// その切り出し位置と、PanelクラスのColorの対応付けテーブル
static const PanelSubTexTableEnt m_PanelSubTexTable[Panel::COLOR_NUM] =
{
    {U"RedBack", U"RedMark"},
    {U"GreenBack", U"GreenMark"},
    {U"WaterBlueBack", U"WaterBlueMark"},
    {U"YellowBack", U"YellowMark"},
    {U"PurpleBack", U"PurpleMark"},
    {U"BlueBack", U"BlueMark"},
    // 色がないので描写なし。空の文字列を指定
    {U"", U""}                         
};

PanelDrawer2D::PanelDrawer2D()
	: m_DrawSetting()
{
    m_PanelTexture.ReadSetting(U"setting/drawpanel_2d.xml");
    m_OjyamaTexture.ReadSetting(U"setting/ojyama_2d.xml");
}

PanelDrawer2D::~PanelDrawer2D()
{}

void PanelDrawer2D::SetDrawSetting(const PanelDrawer2D::DrawSetting& draw_setting) 
{
	m_DrawSetting = draw_setting;
}

void PanelDrawer2D::DrawPanels(const GameLogic& gamelogic)
{
	const PanelContainer& pcont = gamelogic.GetPanelContainer();
	for (int y = pcont.FieldNextLine(); y <= pcont.FieldTop(); ++y) {
		for (int x = pcont.FieldLeft(); x <= pcont.FieldRight(); ++x) {
			drawPanel(gamelogic, x, y);
		}
	}
}

void PanelDrawer2D::drawPanel(const GameLogic& gamelogic, int x, int y)
{
	const PanelContainer& pcont = gamelogic.GetPanelContainer();
	const Panel& panel = pcont.GetPanel(x, y);

	int draw_x = calculateDrawPos_X(gamelogic, panel, x);
	int draw_y = calculateDrawPos_Y(gamelogic, panel, y);
    
    const s3d::String& back_id = m_PanelSubTexTable[panel.color].back_identifier;
    const s3d::String& mark_id = m_PanelSubTexTable[panel.color].mark_identifier;

    // delete after wait のときは、
    // パネルが消えた直後で、スペース表示をしたいので表示させない
    if (panel.type == Panel::TYPE_PANEL && panel.state != Panel::STATE_DELETE_AFTER_WAIT) {

        // ネクストパネルは少し暗くして描写
        if (y == pcont.FieldNextLine()) {
            m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y, s3d::Color(0x50, 0x50, 0x50));
            m_PanelTexture.SubTexture(mark_id).draw(draw_x, draw_y, s3d::Color(0x50, 0x50, 0x50));
        }
        // それ以外はそのまま
        else {
            m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y);
            m_PanelTexture.SubTexture(mark_id).draw(draw_x, draw_y);
        }
    }

    if (panel.type == Panel::TYPE_OJYAMA) {
        if (panel.state == Panel::STATE_UNCOMPRESS_BEFORE_WAIT) {
            s3d::Rect(draw_x, draw_y, 40, 40).draw(s3d::Color(0x00, 0x00, 0xFF));
        }
        else if (panel.state == Panel::STATE_UNCOMPRESS) {
            s3d::Rect(draw_x, draw_y, 40, 40).draw(s3d::Color(0x00, 0x00, 0xFF));
        }
        else if (panel.state == Panel::STATE_UNCOMPRESS_AFTER_WAIT) {
            if (panel.is_mark_be_panel) {
                m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y);
                m_PanelTexture.SubTexture(mark_id).draw(draw_x, draw_y);
            }
            else {
                s3d::TextureRegion subtexture = getOjyamaSubTexture(gamelogic, x, y);
                subtexture.draw(draw_x, draw_y);
            }
        }
        else {
            s3d::TextureRegion subtexture = getOjyamaSubTexture(gamelogic, x, y);
            subtexture.draw(draw_x, draw_y);
        }
    }
}

s3d::TextureRegion PanelDrawer2D::getOjyamaSubTexture(const GameLogic& gamelogic, int x, int y)
{
    const PanelContainer& pcont = gamelogic.GetPanelContainer();
    const Panel& panel = pcont.GetPanel(x, y);
    OjyamaInfo ojyama = pcont.GetOjyamaInfo(x, y);

    s3d::TextureRegion region;

    switch (ojyama.GetPart(x, y)) {
    case OjyamaInfo::OneLine_Left:
        region = m_OjyamaTexture.SubTexture(U"OneLine_Left");
        break;
    case OjyamaInfo::OneLine_Center:
        region = m_OjyamaTexture.SubTexture(U"OneLine_Center");
        break;
    case OjyamaInfo::OneLine_Right:
        region = m_OjyamaTexture.SubTexture(U"OneLine_Right");
        break;
    case OjyamaInfo::MultiLine_TopLeft:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_TopLeft");
        break;
    case OjyamaInfo::MultiLine_TopCenter:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_TopCenter");
        break;
    case OjyamaInfo::MultiLine_TopRight:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_TopRight");
        break;
    case OjyamaInfo::MultiLine_MiddleLeft:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_MiddleLeft");
        break;
    case OjyamaInfo::MultiLine_MiddleCenter:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_MiddleCenter");
        break;
    case OjyamaInfo::MultiLine_MiddleRight:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_MiddleRight");
        break;
    case OjyamaInfo::MultiLine_BottomLeft:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_BottomLeft");
        break;
    case OjyamaInfo::MultiLine_BottomCenter:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_BottomCenter");
        break;
    case OjyamaInfo::MultiLine_BottomRight:
        region = m_OjyamaTexture.SubTexture(U"MultiLine_BottomRight");
        break;
    case OjyamaInfo::OutRange:
    default:
        break;
    }

    return region;
}

int PanelDrawer2D::calculateDrawPos_X(const GameLogic& gamelogic, const Panel& panel, int x)
{
    // パネル交換中の移動位置を計算
	int swapping_count_max = gamelogic.GetFieldSetting().SwappingCountMax();
	double swapping_ratio = static_cast<double>(panel.swapping_count) / swapping_count_max;
	int swapping_dx = panel.move_from.x * (1.0 - swapping_ratio) * m_DrawSetting.PanelUnit;

	int panel_pos_x = x * m_DrawSetting.PanelUnit;

	return m_DrawSetting.BaseX + panel_pos_x + swapping_dx;
}

int PanelDrawer2D::calculateDrawPos_Y(const GameLogic& gamelogic, const Panel& panel, int y)
{
    // パネル落下中の移動位置を計算
	int fall_count_max = gamelogic.GetFieldSetting().FallCountMax();
	double falling_ratio = static_cast<double>(panel.fall_count) / fall_count_max;
	int falling_dy = panel.move_from.y * (1.0 - falling_ratio) * m_DrawSetting.PanelUnit;

    // パネルせり上がり中の移動位置を計算
	int seriagari_max = gamelogic.GetFieldSetting().SeriagariCountMax();
	int seriagari_dy = (static_cast<double>(gamelogic.GetGameState().SeriagariCount()) / seriagari_max) * m_DrawSetting.PanelUnit;

	int panel_pos_y = y * m_DrawSetting.PanelUnit;

	return m_DrawSetting.BaseY - panel_pos_y - falling_dy - seriagari_dy;
}
