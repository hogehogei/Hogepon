
#include "core/Panel.hpp"


Panel::Panel()
	: 
	delete_before_wait(0),
	delete_wait(0),
	delete_after_wait(0),
	swapping_count(0),
	fall_before_wait(0),
	fall_count(0),
	fall_after_wait(0),
	is_chain_seed(false),
	is_mark_delete(false),
    is_mark_uncompress(false),
    is_mark_be_panel(false),
    uncompress_before_wait(0),
    uncompress_wait(0),
    uncompress_after_wait(0),
    ojyama_basepos_from(),
    ojyama(nullptr),
    type( TYPE_SPACE ),
    state( STATE_NONE ),
    color( COLOR_NONE )
{}

Panel::~Panel() throw()
{}

void Panel::ClearMark()
{
	is_mark_delete   = false;
    is_mark_uncompress = false;
    // is_mark_be_panel はクリアしない
}

void Panel::Reset()
{
	move_from		   = PanelPos(0, 0);
    delete_before_wait = 0;
    delete_wait        = 0;
    delete_after_wait  = 0;
    swapping_count     = 0;
    fall_before_wait   = 0;
    fall_count         = 0;
    fall_after_wait    = 0;
    is_chain_seed      = false;
	is_mark_delete     = false;
    
    is_mark_be_panel       = false;
    uncompress_before_wait = 0;
    uncompress_wait        = 0;
    uncompress_after_wait  = 0;
    ojyama_basepos_from = PanelPos(0, 0);

    // type, state, color, お邪魔パネルの情報は
    // 変更しない！
}

