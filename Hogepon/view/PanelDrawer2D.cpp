
#include "view/PanelDrawer2D.hpp"

struct PanelSubTexTableEnt
{
    s3d::String back_identifier;
    s3d::String mark_identifier;
};

// �p�l���̉摜�͈ꖇ�̃e�N�X�`������ꕔ����؂�o���ĕ`�悷��̂ŁA
// �؂�o���ʒu��ݒ�t�@�C�������Ă���
// ���̐؂�o���ʒu�ƁAPanel�N���X��Color�̑Ή��t���e�[�u��
static const PanelSubTexTableEnt m_PanelSubTexTable[Panel::COLOR_NUM] =
{
    {U"RedBack", U"RedMark"},
    {U"GreenBack", U"GreenMark"},
    {U"WaterBlueBack", U"WaterBlueMark"},
    {U"YellowBack", U"YellowMark"},
    {U"PurpleBack", U"PurpleMark"},
    {U"BlueBack", U"BlueMark"},
    // �F���Ȃ��̂ŕ`�ʂȂ��B��̕�������w��
    {U"", U""}                         
};

PanelDrawer2D::PanelDrawer2D()
	: m_DrawSetting()
{
    m_PanelTexture.ReadSetting(U"setting/drawpanel_2d.xml");
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

    if (panel.type == Panel::TYPE_PANEL && panel.state != Panel::STATE_DELETE_AFTER_WAIT) {
        const s3d::String& back_id = m_PanelSubTexTable[panel.color].back_identifier;
        const s3d::String& mark_id = m_PanelSubTexTable[panel.color].mark_identifier;

        // �ŉ��i�̉��i�l�N�X�g�p�l���j�͏����Â����ĕ`��
        if (y == pcont.FieldNextLine()) {
            m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y, s3d::Color(0x50, 0x50, 0x50));
            m_PanelTexture.SubTexture(mark_id).draw(draw_x, draw_y, s3d::Color(0x50, 0x50, 0x50));
        }
        // ����ȊO�͂��̂܂�
        else {
            m_PanelTexture.SubTexture(back_id).draw(draw_x, draw_y);
            m_PanelTexture.SubTexture(mark_id).draw(draw_x, draw_y);
        }
    }
}

int PanelDrawer2D::calculateDrawPos_X(const GameLogic& gamelogic, const Panel& panel, int x)
{
    // �p�l���������̈ړ��ʒu���v�Z
	int swapping_count_max = gamelogic.GetFieldSetting().SwappingCountMax();
	double swapping_ratio = static_cast<double>(panel.swapping_count) / swapping_count_max;
	int swapping_dx = panel.move_from.x * (1.0 - swapping_ratio) * m_DrawSetting.PanelUnit;

	int panel_pos_x = x * m_DrawSetting.PanelUnit;

	return m_DrawSetting.BaseX + panel_pos_x + swapping_dx;
}

int PanelDrawer2D::calculateDrawPos_Y(const GameLogic& gamelogic, const Panel& panel, int y)
{
    // �p�l���������̈ړ��ʒu���v�Z
	int fall_count_max = gamelogic.GetFieldSetting().FallCountMax();
	double falling_ratio = static_cast<double>(panel.fall_count) / fall_count_max;
	int falling_dy = panel.move_from.y * (1.0 - falling_ratio) * m_DrawSetting.PanelUnit;

    // �p�l������オ�蒆�̈ړ��ʒu���v�Z
	int seriagari_max = gamelogic.GetFieldSetting().SeriagariCountMax();
	int seriagari_dy = (static_cast<double>(gamelogic.GetGameState().SeriagariCount()) / seriagari_max) * m_DrawSetting.PanelUnit;

	int panel_pos_y = y * m_DrawSetting.PanelUnit;

	return m_DrawSetting.BaseY - panel_pos_y - falling_dy - seriagari_dy;
}
