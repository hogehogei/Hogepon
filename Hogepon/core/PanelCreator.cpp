
#include "core/PanelCreator.hpp"
#include "core/Panel.hpp"

PanelCreator::PanelCreator()
    : m_Random( 0 )
{}

PanelCreator::~PanelCreator()
{}

void PanelCreator::SetSeed(uint32_t seed)
{
	m_Random.seed(seed);
}

Panel PanelCreator::Create()
{
    Panel panel;
    panel.type  = Panel::TYPE_PANEL;
    panel.state = Panel::STATE_DEFAULT;
    panel.color = static_cast<Panel::Color>( m_Random() % 6 );

    return panel;
}

