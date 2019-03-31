 

#include <algorithm>

#include <cassert>
#include "core/Panel.hpp"
#include "core/PanelContainer.hpp"


PanelContainer::PanelContainer()
{}

PanelContainer::~PanelContainer() throw()
{}

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

void PanelContainer::NewLine()
{
	// 一段上のパネルにコピー
	for (int y = FieldTop() - 1; y >= FieldNextLine(); --y) {
		for (int x = FieldLeft(); x <= FieldRight(); ++x) {
			Panel& panel = GetPanel(x, y);
			Panel& above_panel = GetPanel(x, y + 1);
			above_panel = panel;
		}
	}

	// 一番下の段には新しいパネルをセット
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