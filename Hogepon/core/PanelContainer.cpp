 

#include <algorithm>

#include <cassert>
#include "core/Panel.hpp"
#include "core/PanelContainer.hpp"

void PanelContainer::Initialize(int w, int h, uint32_t seed)
{
	m_Width      = w;
	m_Height     = h;
	m_RealWidth  = w;
	m_RealHeight = h + 2;
	m_Panels     = s3d::Array<Panel>(m_RealWidth * m_RealHeight);

	m_PanelCreator.SetSeed(seed);
	
	initializeField();
}

Panel& PanelContainer::GetPanel( int x, int y )
{
    //assert(Is_InField( x, y ) && "panel pos out of range!" );
    
    return m_Panels[ y * m_RealWidth + x ];
}

const Panel& PanelContainer::GetPanel(int x, int y) const
{
	//assert(Is_InField(x, y) && "panel pos out of range!");

	return m_Panels[y * m_RealWidth + x];
}

Panel& PanelContainer::GetUnderPanel(int x, int y)
{
	int under_y = std::max(0, y - 1);
	return m_Panels[under_y * m_RealWidth + x];
}

void PanelContainer::SetPanel(int x, int y, const Panel& panel)
{
	//assert(Is_InField(x, y) && "panel pos out of range!");

	m_Panels[y * m_RealWidth + x] = panel;
}

PanelCreator& PanelContainer::GetPanelCreator()
{
    return m_PanelCreator;
}

OjyamaPanel PanelContainer::GetOjyamaPanel(uint32_t ojyama_id)
{
    return OjyamaPanel(this, ojyama_id);
}

PanelContainer::OjyamaPanelVec PanelContainer::GetOjyamaPanelListOnField()
{
    std::vector<uint32_t> find_ojyama_list;
    OjyamaPanelVec ojyama_list;

    for (int y = FieldBottom(); y <= FieldTop(); ++y) {
        for (int x = FieldLeft(); x <= FieldRight(); ++x) {
            const Panel& panel = GetPanel(x, y);

            if (panel.type == Panel::TYPE_OJYAMA) {
                uint32_t id = panel.ojyama->id;

                auto result = std::find( find_ojyama_list.begin(), find_ojyama_list.end(), id );
                if (result == find_ojyama_list.end()) {
                    find_ojyama_list.push_back(id);

                    OjyamaPanel ojyama(this, x, y, panel);
                    ojyama_list.push_back(ojyama);
                }
            }
        }
    }

    return ojyama_list;
}

void PanelContainer::ClearOjyamaUpdated()
{
    for (int y = FieldBottom(); y <= FieldNewOjyamaLine(); ++y) {
        for (int x = FieldLeft(); x <= FieldRight(); ++x) {
            Panel& panel = GetPanel(x, y);
            if (panel.type == Panel::TYPE_OJYAMA && panel.ojyama) {
                panel.ojyama->is_updated = false;
            }
        }
    }
}

bool PanelContainer::Is_InFieldCursor(int x, int y) const
{
	if (x >= FieldLeft() && x <= ( FieldRight() - 1 ) &&
		y >= FieldBottom() && y <= FieldTop() )
	{
		return true;
	}

	return false;
}

bool PanelContainer::Is_InField(int x, int y) const
{
	if (x >= FieldLeft() && x <= FieldRight() &&
		y >= FieldBottom() && y <= FieldTop() )
	{
		return true;
	}

	return false;
}

void PanelContainer::FallOjyamaLine(bool dequeue_enable)
{
    m_OjyamaCreator.FallOjyamaLine(this, dequeue_enable);
}

void PanelContainer::OjyamaAppend(int chain_count, int doujikeshi_count)
{
    m_OjyamaCreator.Append(chain_count, doujikeshi_count);
}

void PanelContainer::NewLine()
{
	// ��i��̃p�l���ɃR�s�[
	for (int y = FieldTop() - 1; y >= FieldNextLine(); --y) {
		for (int x = FieldLeft(); x <= FieldRight(); ++x) {
			Panel& panel = GetPanel(x, y);
			Panel& above_panel = GetPanel(x, y + 1);
			above_panel = panel;
		}
	}

	// ��ԉ��̒i�ɂ͐V�����p�l�����Z�b�g
	int newline = FieldBottom() - 1;
	for (int x = FieldLeft(); x <= FieldRight(); ++x) {
		Panel& bottom_panel = GetPanel(x, newline);
		Panel new_panel;
		do {
			new_panel = m_PanelCreator.Create();
			
		} while (x > FieldLeft() && new_panel.color == GetPanel(x - 1, newline).color);

		bottom_panel = new_panel;
	}
}

void PanelContainer::initializeField()
{
	for (int y = 0; y < 7; ++y) {
		for (int x = 0; x < m_RealWidth; ++x) {
			SetPanel(x, y, m_PanelCreator.Create());
		}
	}
}