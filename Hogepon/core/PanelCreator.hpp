
#ifndef   PANEPON_PANEL_CREATOR_HPP
#define   PANEPON_PANEL_CREATOR_HPP

#include <random>
#include "core/Panel.hpp"

class PanelCreator
{
public:

    PanelCreator();
	~PanelCreator() noexcept;

	void SetSeed(uint32_t seed);
    Panel Create();
    
private:

	std::mt19937 m_Random;
};

#endif   //  PANEPON_PANEL_CREATOR_HPP
