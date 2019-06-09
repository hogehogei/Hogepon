
#include "view/PanelDrawer2D.hpp"

struct PanelSubTexTableEnt
{
    s3d::String back_identifier;
    s3d::String mark_identifier;
    s3d::String face_identifier;
};

// パネルの画像は一枚のテクスチャから一部分を切り出して描画するので、
// 切り出し位置を設定ファイル化している
// その切り出し位置と、PanelクラスのColorの対応付けテーブル
static const PanelSubTexTableEnt m_PanelSubTexTable[Panel::COLOR_NUM] =
{
    {U"RedBack"      , U"RedMark",       U"RedFace"},
    {U"GreenBack"    , U"GreenMark",     U"GreenFace"},
    {U"WaterBlueBack", U"WaterBlueMark", U"WaterBlueFace"},
    {U"YellowBack"   , U"YellowMark",    U"YellowFace"},
    {U"PurpleBack"   , U"PurpleMark",    U"PurpleFace"},
    {U"BlueBack"     , U"BlueMark",      U"BlueFace"},
    // 色がないので描写なし。空の文字列を指定
    {U""             , U"",              U""}
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

    // delete after wait のときは、
    // パネルが消えた直後で、スペース表示をしたいので表示させない
    if (panel.type == Panel::TYPE_PANEL && panel.state != Panel::STATE_DELETE_AFTER_WAIT) {

        if (panel.state == Panel::STATE_DELETE_BEFORE_WAIT) {
            drawPanelBeforeDelete(gamelogic, panel, draw_x, draw_y);
        }
        // 消去中は顔を表示したい
        else if (panel.state == Panel::STATE_DELETE) {
            drawPanelSurprisedFace(gamelogic, panel, draw_x, draw_y);
        }
        else {
            // ネクストパネルは少し暗くして描写
            if (y == pcont.FieldNextLine()) {
                m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y, s3d::Color(0x50, 0x50, 0x50));
                drawPanelMark(gamelogic, panel, draw_x, draw_y, s3d::Color(0x50, 0x50, 0x50));
            }
            // それ以外はそのまま
            else {
                m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y);
                drawPanelMark(gamelogic, panel, draw_x, draw_y, s3d::Color(0xFF, 0xFF, 0xFF));
            }
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
                drawPanelMark(gamelogic, panel, draw_x, draw_y, s3d::Color(0xFF, 0xFF, 0xFF));
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

void PanelDrawer2D::drawPanelMark(const GameLogic& gamelogic, const Panel& panel, int draw_pixel_x, int draw_pixel_y, const s3d::Color& diffuse)
{
    const GameFieldSetting& settings = gamelogic.GetFieldSetting();
    const s3d::String& mark_id = m_PanelSubTexTable[panel.color].mark_identifier;

    // 落下時にパネルのマークを弾ませるような表示をする
    // 具体的には、マークのテクスチャだけ少しの間縮ませて表示する
    // 縮ませる間(0 - 1とする) を 0 - Pi と変換し、 その sine カーブを利用した。
    // （sine カーブの 0 - 90℃の範囲で、凹となっているような感じ）
    double fall_after_wait_progress = panel.fall_after_wait / static_cast<double>(settings.FallAfterWaitMax());
    double compress_curve = s3d::Sin(fall_after_wait_progress * s3d::Math::Pi);
    double compress_ratio = compress_curve * 0.3;
    int animated_pixel_y = draw_pixel_y + (m_DrawSetting.PanelSize * compress_ratio);

    m_PanelTexture.SubTexture(mark_id).scaled(1.0, 1.0 - compress_ratio).draw(draw_pixel_x, animated_pixel_y, diffuse);
}

void PanelDrawer2D::drawPanelBeforeDelete(const GameLogic& gamelogic, const Panel& panel, int draw_pixel_x, int draw_pixel_y)
{
    const s3d::String& back_id = m_PanelSubTexTable[panel.color].back_identifier;
    const s3d::String& mark_id = m_PanelSubTexTable[panel.color].mark_identifier;

    int panel_size = m_DrawSetting.PanelSize;
    int blink_period = 8;
    int blink = (panel.delete_before_wait % blink_period) / (blink_period/2);

    if (blink == 0) {
        s3d::Rect(draw_pixel_x, draw_pixel_y, m_DrawSetting.PanelSize, m_DrawSetting.PanelSize).draw(s3d::Color(0xFF, 0xFF, 0xFF));
    }
    else {
        m_PanelTexture.SubTexture(back_id).draw(draw_pixel_x, draw_pixel_y);
    }
    m_PanelTexture.SubTexture(mark_id).draw(draw_pixel_x, draw_pixel_y);
}

void PanelDrawer2D::drawPanelSurprisedFace(const GameLogic& gamelogic, const Panel& panel, int draw_pixel_x, int draw_pixel_y)
{
    const s3d::String& back_id = m_PanelSubTexTable[panel.color].back_identifier;
    const s3d::String& face_id = m_PanelSubTexTable[panel.color].face_identifier;

    m_PanelTexture.SubTexture(back_id).draw(draw_pixel_x, draw_pixel_y);
    m_PanelTexture.SubTexture(face_id).draw(draw_pixel_x, draw_pixel_y);
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
	int swapping_dx = panel.move_from.x * (1.0 - swapping_ratio) * m_DrawSetting.PanelSize;

	int panel_pos_x = x * m_DrawSetting.PanelSize;

	return m_DrawSetting.BaseX + panel_pos_x + swapping_dx;
}

int PanelDrawer2D::calculateDrawPos_Y(const GameLogic& gamelogic, const Panel& panel, int y)
{
    // パネル落下中の移動位置を計算
	int fall_count_max = gamelogic.GetFieldSetting().FallCountMax();
	double falling_ratio = static_cast<double>(panel.fall_count) / fall_count_max;
	int falling_dy = panel.move_from.y * (1.0 - falling_ratio) * m_DrawSetting.PanelSize;

    // パネルせり上がり中の移動位置を計算
	int seriagari_max = gamelogic.GetFieldSetting().SeriagariCountMax();
	int seriagari_dy = (static_cast<double>(gamelogic.GetGameState().SeriagariCount()) / seriagari_max) * m_DrawSetting.PanelSize;

	int panel_pos_y = y * m_DrawSetting.PanelSize;

	return m_DrawSetting.BaseY - panel_pos_y - falling_dy - seriagari_dy;
}
