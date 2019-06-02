
#include <Siv3D.hpp>
#include "view/FieldDrawer.hpp"
#include "view/PanelDrawer2D.hpp"

FieldDrawer::FieldDrawer()
	: m_PanelDrawer2D(),
      m_CursorTexture(),
      m_FrameTexture(),
      m_PanelSize(0),
      m_LowerLeft_X(0),
      m_LowerLeft_Y(0)
{
    readDrawSetting(U"setting/fielddraw_1P.xml");

	m_PanelDrawer2D.SetDrawSetting(PanelDrawer2D::DrawSetting(m_LowerLeft_X, m_LowerLeft_Y, m_PanelSize));
    m_CursorTexture.ReadSetting(U"setting/cursor.xml");
    m_FrameTexture = s3d::Texture(U"image/frame.png");
}

FieldDrawer::~FieldDrawer()
{}

void FieldDrawer::readDrawSetting(const s3d::String& filepath)
{
    s3d::XMLReader xml(filepath);
    
    m_PanelSize   = s3d::Parse<int>(xml.attribute(U"PanelSize").value_or(U"0"));
    m_LowerLeft_X = s3d::Parse<int>(xml.attribute(U"LowerLeft_X").value_or(U"0"));
    m_LowerLeft_Y = s3d::Parse<int>(xml.attribute(U"LowerLeft_Y").value_or(U"0"));
}

void FieldDrawer::Draw(const GameLogic& gamelogic)
{
    int frame_height = m_PanelSize * gamelogic.GetFieldSetting().Height();
    int frame_width  = m_PanelSize * gamelogic.GetFieldSetting().Width();

    Rect(m_LowerLeft_X, (m_LowerLeft_Y - frame_height), frame_width, frame_height).draw(
        { Color(0x00, 0x40, 0xFF, 0x80),
          Color(0x00, 0x40, 0xFF, 0x80),
          Color(0x00, 0x80, 0xFF, 0x80),
          Color(0x00, 0x80, 0xFF, 0x80) });

    Rect(m_LowerLeft_X-2, (m_LowerLeft_Y - frame_height )-2, frame_width+4, frame_height+4).drawFrame(2, Color(0x00, 0xD5, 0xD5));
    m_PanelDrawer2D.DrawPanels(gamelogic);
    Rect(m_LowerLeft_X-3, m_LowerLeft_Y, frame_width+6, m_PanelSize).draw(Color(0x00, 0xD5, 0xD5));

    drawCursor(gamelogic);
}


void FieldDrawer::drawCursor(const GameLogic& gamelogic)
{
	int cursor_x = gamelogic.GetCursorPos().cx;
	int cursor_y = gamelogic.GetCursorPos().cy;

    // パネルせり上がり中の移動位置を計算
    int seriagari_max = gamelogic.GetFieldSetting().SeriagariCountMax();
    int seriagari_dy = (static_cast<double>(gamelogic.GetGameState().SeriagariCount()) / seriagari_max) * m_PanelSize;

    int draw_x = m_LowerLeft_X + (cursor_x * m_PanelSize) - 7;
    int draw_y = m_LowerLeft_Y - (cursor_y * m_PanelSize) - 7 - seriagari_dy;

    m_CursorTexture.SubTexture(U"Cursor").draw(draw_x, draw_y);
}