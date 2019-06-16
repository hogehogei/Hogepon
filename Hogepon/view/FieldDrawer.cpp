
#include <Siv3D.hpp>
#include "view/FieldDrawer.hpp"
#include "view/PanelDrawer2D.hpp"
#include "util/EventManager.hpp"
#include "util/IEventHandler.hpp"
#include "event/Event.hpp"

FieldDrawer::FieldDrawer()
	: m_PanelDrawer2D(),
      m_CursorTexture(),
      m_FrameTexture(),
      m_PanelSize(0),
      m_LowerLeft_X(0),
      m_LowerLeft_Y(0),
      m_EasingShake(),
      m_ShakeAmptitude_Y(0)
{
    readDrawSetting(U"setting/fielddraw_1P.xml");

	m_PanelDrawer2D.SetDrawSetting(PanelDrawer2D::DrawSetting(m_LowerLeft_X, m_LowerLeft_Y, m_PanelSize));
    m_CursorTexture.ReadSetting(U"setting/cursor.xml");
    m_FrameTexture = s3d::Texture(U"image/frame.png");

    m_EasingShake = EasingShake(15.0, 10.0, 500);

    exlib::IEventHandlerPtr event_handler = std::make_shared<exlib::EventHandlerBase<FieldDrawer>>("FieldDrawerEventHandler", this);
    exlib::EventManager::Instance().AddHandler(OjyamaFallEvent::sk_EventType, event_handler);
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

void FieldDrawer::StartShakeField()
{
    m_EasingShake.Start();
}

void FieldDrawer::Draw(const GameLogic& gamelogic)
{
    updateShakeAmptitude();

    int frame_height = m_PanelSize * gamelogic.GetFieldSetting().Height();
    int frame_width  = m_PanelSize * gamelogic.GetFieldSetting().Width();
    int lowerleft_x = getPosLowerLeftX();
    int lowerleft_y = getPosLowerLeftY();

    Rect(lowerleft_x, (lowerleft_y - frame_height), frame_width, frame_height).draw(
        { Color(0x00, 0x40, 0xFF, 0x80),
          Color(0x00, 0x40, 0xFF, 0x80),
          Color(0x00, 0x80, 0xFF, 0x80),
          Color(0x00, 0x80, 0xFF, 0x80) });

    Rect(lowerleft_x -2, (lowerleft_y - frame_height )-2, frame_width+4, frame_height+4).drawFrame(2, Color(0x00, 0xD5, 0xD5));
    m_PanelDrawer2D.SetDrawSetting(PanelDrawer2D::DrawSetting(lowerleft_x, lowerleft_y, m_PanelSize));
    m_PanelDrawer2D.DrawPanels(gamelogic);
    Rect(lowerleft_x -3, lowerleft_y, frame_width+6, m_PanelSize).draw(Color(0x00, 0xD5, 0xD5));

    drawCursor(gamelogic);
}


void FieldDrawer::drawCursor(const GameLogic& gamelogic)
{
	int cursor_x = gamelogic.GetCursorPos().cx;
	int cursor_y = gamelogic.GetCursorPos().cy;
    int lowerleft_x = getPosLowerLeftX();
    int lowerleft_y = getPosLowerLeftY();

    // パネルせり上がり中の移動位置を計算
    int seriagari_max = gamelogic.GetFieldSetting().SeriagariCountMax();
    int seriagari_dy = (static_cast<double>(gamelogic.GetGameState().SeriagariCount()) / seriagari_max) * m_PanelSize;

    int draw_x = lowerleft_x + (cursor_x * m_PanelSize) - 7;
    int draw_y = lowerleft_y - (cursor_y * m_PanelSize) - 7 - seriagari_dy;

    m_CursorTexture.SubTexture(U"Cursor").draw(draw_x, draw_y);
}

int FieldDrawer::getPosLowerLeftX() const
{
    return m_LowerLeft_X;
}

int FieldDrawer::getPosLowerLeftY() const
{
    return m_LowerLeft_Y + m_ShakeAmptitude_Y;
}

void FieldDrawer::updateShakeAmptitude()
{
    m_ShakeAmptitude_Y = m_EasingShake.Amptitude();
}

bool FieldDrawer::EventHandler(FieldDrawer* self, const exlib::IEvent& event)
{
    if (event.EventType() == OjyamaFallEvent::sk_EventType) {
        self->StartShakeField();
    }

    return true;
}
