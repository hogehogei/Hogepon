
#include <random>
#include <Siv3D.hpp>
#include "core/GameLogic.hpp"

GameLogic::GameLogic( PlayerID id,
                      GameMode mode,
					  const s3d::String& setting_filepath)
    : m_PlayerID( id ),
      m_GameMode( mode ),
      m_PanelContainer(),
      m_FieldSetting( setting_filepath ),
      m_State()
{
	std::random_device rand{};

	// パネル配置初期化
	m_PanelContainer.Initialize(
		m_FieldSetting.Width(),
		m_FieldSetting.Height(),
		static_cast<uint32_t>(rand())
	);

	m_Cursor.cx = 3;
	m_Cursor.cy = 6;
}

GameLogic::~GameLogic() throw()
{}

void GameLogic::Update()
{
	if (m_State.IsGameOver()) {
		return;
	}

	s3d::Print << m_Cursor.cx << U" " << m_Cursor.cy;
    
    // 連鎖が終わったかどうか
    bool isEndChain = true;
	
	// フィールドの状態をリセットしておく
	m_State.SetState(GameState::STATE_AUTO_SERIAGARI);
	m_State.ClearState(GameState::STATE_SERIAGARI | GameState::STATE_DELETE_PANEL | GameState::STATE_UNCOMPRESS_OJYAMA);
	clearPanelMark();

	// 入力に対する処理
	processInput();
	// フィールドがピンチか調べる
	check_IsPlayerPinch();
    
    //
    // パネルの状態をupdate
    // 下から順番に調べていく
    // なぜ下から調べるかというと、パネルが落ちるときに
    // 上から調べるとずれが生じるから
    // 
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y ) {
        for( int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x ){
            Panel& panel = m_PanelContainer.GetPanel( x, y );

            // 連鎖が終了したかチェックします
            if( panel.is_chain_seed ){
                isEndChain = false;
            }
			/*
            if( panel.type == Panel::TYPE_OJYAMA &&
                ojyamaID_set.find( panel.ojyama_id ) == ojyamaID_set.end() ){
                ojyamaID_set.insert( panel.ojyama_id );
                updateOjyama( panel.ojyama_id );
            }
            if( panel.type == Panel::TYPE_OJYAMA ){
                continue;
            }
            // おジャマ解凍直後なら処理しない
            if( panel.is_uncompress ){
                panel.is_uncompress = false;
                continue;
            }
			*/
            
            // フィールドの状態を更新
            updateState( x, y );
        }
    }

	doSeriagari();
	deletePanel();
	judgeGameOver();

    // 連鎖終了ならチェインカウントを戻そう
    if( isEndChain ){
		m_State.EndChain();
    }
}

void GameLogic::clearPanelMark()
{
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
		for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
			Panel& panel = m_PanelContainer.GetPanel(x, y);
			panel.ClearMark();
		}
	}
}

void GameLogic::processInput()
{
	if (s3d::KeySpace.down()) {
		if (swapPanel()) {
			// 交換に成功したので、イベント送出すること！
		}
	}
	if (s3d::KeyUp.down()) {
		if (m_PanelContainer.Is_InFieldCursor(m_Cursor.cx, m_Cursor.cy + 1)) {
			m_Cursor.cy += 1;
		}
	}
	if (s3d::KeyDown.down()) {
		if (m_PanelContainer.Is_InFieldCursor(m_Cursor.cx, m_Cursor.cy - 1)) {
			m_Cursor.cy -= 1;
		}
	}
	if (s3d::KeyLeft.down()) {
		if (m_PanelContainer.Is_InFieldCursor(m_Cursor.cx - 1, m_Cursor.cy)) {
			m_Cursor.cx -= 1;
		}
	}
	if (s3d::KeyRight.down()) {
		if (m_PanelContainer.Is_InFieldCursor(m_Cursor.cx + 1, m_Cursor.cy)) {
			m_Cursor.cx += 1;
		}
	}
	if (s3d::KeyZ.pressed()) {
		m_State.SetState(GameState::STATE_SERIAGARI);
	}
}

bool GameLogic::swapPanel()
{
	Panel& left = m_PanelContainer.GetPanel(m_Cursor.cx, m_Cursor.cy);
	Panel& right = m_PanelContainer.GetPanel(m_Cursor.cx + 1, m_Cursor.cy);

	if (m_Cursor.cy < m_PanelContainer.FieldTop()) {
		// 交換するパネルの上のパネルが落下直前だったら
		// 交換できない！
		Panel& above_panel_left = m_PanelContainer.GetPanel(m_Cursor.cx, m_Cursor.cy + 1);
		Panel& above_panel_right = m_PanelContainer.GetPanel(m_Cursor.cx + 1, m_Cursor.cy + 1);

		if ((above_panel_left.type == Panel::TYPE_PANEL &&
			above_panel_left.state == Panel::STATE_FALL_BEFORE_WAIT) ||
			(above_panel_right.type == Panel::TYPE_PANEL &&
				above_panel_right.state == Panel::STATE_FALL_BEFORE_WAIT)) {
			return false;
		}
	}

	// 交換できるパネルか？
	bool isValidTypeLeft = left.type == Panel::TYPE_PANEL || left.type == Panel::TYPE_SPACE;
	bool isValidTypeRight = right.type == Panel::TYPE_PANEL || right.type == Panel::TYPE_SPACE;
	bool isValidStateLeft = left.state == Panel::STATE_DEFAULT || left.state == Panel::STATE_NONE || left.state == Panel::STATE_FALL_AFTER_WAIT;
	bool isValidStateRight = right.state == Panel::STATE_DEFAULT || right.state == Panel::STATE_NONE || right.state == Panel::STATE_FALL_AFTER_WAIT;

	if (!(isValidTypeLeft && isValidTypeRight && isValidStateLeft && isValidStateRight)) {
		return false;
	}

	// パネル交換中フラグを立てる
	// TYPE_SPACE パネルも交換状態を持つので
	// 交換が終わったら STATE_NONE に戻すこと！
	left.reset();
	right.reset();
	left.state = Panel::STATE_SWAPPING;
	right.state = Panel::STATE_SWAPPING;
	left.move_from = PanelPos(-1, 0);
	right.move_from = PanelPos(1, 0);

	std::swap(left, right);

	return true;
}

void GameLogic::check_IsPlayerPinch()
{
	m_State.ClearState(GameState::STATE_WARNING | GameState::STATE_DANGER);

	// 危険ゾーンにパネルがあるかチェック
	int line_danger = m_PanelContainer.FieldTop();
	int line_warning = m_PanelContainer.FieldTop() - 3;

	for (int y = line_danger; y >= line_warning; --y) {
		for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
			const Panel& panel = m_PanelContainer.GetPanel(x, y);

			// 落下中以外のパネルがあるならワーニング
			if (panel.type != Panel::TYPE_SPACE &&
				!(panel.state == Panel::STATE_FALL_BEFORE_WAIT || panel.state == Panel::STATE_FALLING)) {
				m_State.SetState(GameState::STATE_WARNING);
				if (x == line_danger) {
					m_State.SetState(GameState::STATE_DANGER);
				}
			}
		}
	}
}

bool GameLogic::canSeriagari()
{
	bool can_seriagari = true;

	// せり上がりできる状態か調べる
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
		for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
			const Panel& panel = m_PanelContainer.GetPanel(x, y);

			if (panel.type != Panel::TYPE_SPACE &&
				!(panel.state == Panel::STATE_DEFAULT || panel.state == Panel::STATE_SWAPPING)) {
				can_seriagari = false;
			}
		}
	}

	return can_seriagari;
}

void GameLogic::doSeriagari()
{
	if (canSeriagari()) {
		if (m_State.IsSeriagari()) {
			m_State.IncSeriagariCount(m_FieldSetting.IncFastSeriagari());
		}
		// TODO お邪魔を落とすタイミングは decWaitTimer() == 0 のとき
		//if( !is_del_or_uncompress ){
			// お邪魔落とせるなら落として！
		//    exlib::EventManager::instance()->queueEvent( fall_ojyama_ev );
		// }
		if (m_State.IsAutoSeriagari() &&
			(!m_State.IsUncompressOjyama() && !m_State.IsDeletePanel())) {
			m_State.IncSeriagariCount(m_FieldSetting.IncAutoSeriagari());
		}

		if (m_State.SeriagariCount() >= m_FieldSetting.SeriagariCountMax()) {
            // せりあがり実施
			m_PanelContainer.NewLine();
            if (m_Cursor.cy <= m_PanelContainer.FieldTop()) {
                m_Cursor.cy += 1;
            }
			m_State.SetSeriagariCount(0);
		}
	}
}

void GameLogic::judgeGameOver()
{    
	// せり上がりされたら待ち時間は消去
	if ( m_State.IsSeriagari() ) {
		m_State.SetSeriagariWaitTimer(0);
	}

    // 連鎖などしてWaitがあるなら優先的に使おう
    if( m_State.SeriagariWaitTimer() > 0 ||
		m_State.IsUncompressOjyama() ||
		m_State.IsDeletePanel()
		){
        m_State.DecSeriagariWaitTimer();
    }
	else {
		// ゲームオーバー寸前？
		if (m_State.IsDanger()) {
			if (m_State.IsSeriagari()) {
				m_State.SetState(GameState::STATE_GAME_OVER);
			}

			m_State.DecGameOverWaitTimer();
			if (m_State.GameOverWaitTimer() <= 0) {
				m_State.SetState(GameState::STATE_GAME_OVER);
			}
		}
		else {
			// ゲームオーバーまでの待ち時間を設定
			m_State.SetGameOverWaitTimer(m_FieldSetting.GameOverTimeLimit());
		}
	}
}

void GameLogic::deletePanel()
{
	//
	//   横に3個以上パネルがくっついているか調べる
	//
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
		for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight() - 2; ) {
			int del_count = 1;

			for (PanelPos pos(x, y), next_pos(x + 1, y);
				pos.x <= m_PanelContainer.FieldRight() - 1;
				++pos.x, ++next_pos.x)
			{
				const Panel& panel = m_PanelContainer.GetPanel(pos.x, pos.y);
				const Panel& next_panel = m_PanelContainer.GetPanel(next_pos.x, next_pos.y);

				bool isValidStatePanel = panel.state == Panel::STATE_DEFAULT || panel.state == Panel::STATE_FALL_AFTER_WAIT;
				bool isValidStateNexPanel = next_panel.state == Panel::STATE_DEFAULT || next_panel.state == Panel::STATE_FALL_AFTER_WAIT;

				if (panel.type == Panel::TYPE_PANEL && isValidStatePanel &&
					next_panel.type == Panel::TYPE_PANEL && isValidStateNexPanel &&
					panel.color == next_panel.color) {
					++del_count;
				}
				else {
					break;
				}
			}

			if (del_count >= 3) {
				for (PanelPos pos(x, y); pos.x < x + del_count; ++pos.x) {
					Panel& delete_panel = m_PanelContainer.GetPanel(pos.x, pos.y);
					if (!delete_panel.is_mark_delete) {
						delete_panel.is_mark_delete = true;
					}
				}
			}

			x += del_count;
		}
	}

	//
	// 縦に3個以上パネルがくっついているか調べる
	//
	for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
		for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop() - 2; ) {
			int del_count = 1;

			for (PanelPos pos(x, y), next_pos(x, y + 1);
				pos.y <= m_PanelContainer.FieldTop() - 1;
				++pos.y, ++next_pos.y)
			{
				const Panel& panel = m_PanelContainer.GetPanel(pos.x, pos.y);
				const Panel& next_panel = m_PanelContainer.GetPanel(next_pos.x, next_pos.y);

				bool isValidStatePanel = panel.state == Panel::STATE_DEFAULT || panel.state == Panel::STATE_FALL_AFTER_WAIT;
				bool isValidStateNexPanel = next_panel.state == Panel::STATE_DEFAULT || next_panel.state == Panel::STATE_FALL_AFTER_WAIT;

				if (panel.type == Panel::TYPE_PANEL && isValidStatePanel &&
					next_panel.type == Panel::TYPE_PANEL &&  isValidStateNexPanel &&
					panel.color == next_panel.color) {
					++del_count;
				}
				else {
					break;
				}
			}

			if (del_count >= 3) {
				for (PanelPos pos(x, y); pos.y < y + del_count; ++pos.y) {
					Panel& delete_panel = m_PanelContainer.GetPanel(pos.x, pos.y);
					if (!delete_panel.is_mark_delete) {
						delete_panel.is_mark_delete = true;
					}
				}
			}

			y += del_count;
		}
	}


	changeState_PanelsDeleting();
}

void GameLogic::changeState_PanelsDeleting()
{
	int delete_panel_num = countDeleteMarkPanel();
	int delete_wait_per_panel = m_FieldSetting.WaitTimePanelDel();
    int delete_time_max = delete_wait_per_panel * delete_panel_num;
    int delete_panel_cnt = 0;

	bool is_chain = false;

	for (int y = m_PanelContainer.FieldTop(); y >= m_PanelContainer.FieldBottom(); --y) {
		for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
			Panel& panel = m_PanelContainer.GetPanel(x, y);

			if (panel.is_mark_delete) {
                ++delete_panel_cnt;

				panel.state = Panel::STATE_DELETE_BEFORE_WAIT;
				panel.delete_before_wait = 0;
                panel.delete_wait = delete_wait_per_panel * delete_panel_cnt;
                panel.delete_after_wait = delete_time_max - panel.delete_wait + 1;

				if (panel.is_chain_seed) {
					is_chain = true;
				}
			}
		}
	}

	if (delete_panel_num >= 3) {
		m_State.Doujikeshi(delete_panel_num);
	}
	if (is_chain) {
		m_State.IncChainCount();
	}
}

int GameLogic::countDeleteMarkPanel()
{
	int delete_panel_count = 0;
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
		for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
			if (m_PanelContainer.GetPanel(x, y).is_mark_delete) {
				++delete_panel_count;
			}
		}
	}

	return delete_panel_count;
}

void GameLogic::update_PanelDefault(int x, int y, Panel& panel)
{
	const Panel& under_panel = m_PanelContainer.GetUnderPanel(x, y);
	if (under_panel.type == Panel::TYPE_SPACE &&
		under_panel.state != Panel::STATE_SWAPPING) {
		panel.fall_before_wait = 0;
		panel.state = Panel::STATE_FALL_BEFORE_WAIT;
	}
	if (under_panel.state == Panel::STATE_FALL_BEFORE_WAIT) {
		panel.is_chain_seed = under_panel.is_chain_seed;
		panel.fall_before_wait = under_panel.fall_before_wait;
		panel.state = Panel::STATE_FALL_BEFORE_WAIT;
	}
}

void GameLogic::update_PanelSwapping(int x, int y, Panel& panel)
{
	panel.swapping_count += m_FieldSetting.IncSwappingCount();

	if (panel.swapping_count >= m_FieldSetting.SwappingCountMax()) {
		panel.swapping_count = 0;
		panel.move_from = PanelPos(0, 0);

		if (panel.type == Panel::TYPE_PANEL) {
			const Panel& under_panel = m_PanelContainer.GetUnderPanel(x, y);
			if (under_panel.type == Panel::TYPE_SPACE) {
				panel.fall_before_wait = 0;
				panel.state = Panel::STATE_FALL_BEFORE_WAIT;
			}
			else if (under_panel.type == Panel::TYPE_PANEL) {
				panel.reset();
				panel.state = Panel::STATE_DEFAULT;
			}
		}
		else if (panel.type == Panel::TYPE_SPACE) {
			panel.state = Panel::STATE_NONE;
		}
		else {
			// ありえない！
			assert(0 && "invalid panel type!");
		}
	}
}

void GameLogic::update_PanelFallBeforeWait(int x, int y, Panel& panel)
{
	Panel& under_panel = m_PanelContainer.GetUnderPanel(x, y);
	panel.fall_before_wait += m_FieldSetting.IncFallBeforeWait();

	// パネル解凍直後は下がスペースでないこともありえるのでチェック
	if (panel.fall_before_wait >= m_FieldSetting.FallBeforeWaitMax()) {
		if (under_panel.type == Panel::TYPE_SPACE) {
			panel.fall_before_wait = 0;
			panel.state = Panel::STATE_FALLING;
			panel.move_from = PanelPos(0, 1);
			std::swap(panel, under_panel);
		}
		else {
			panel.state = Panel::STATE_FALL_AFTER_WAIT;
			panel.fall_after_wait = 0;
		}
	}
}

void GameLogic::update_PanelFalling(int x, int y, Panel& panel)
{
	panel.fall_count += m_FieldSetting.IncFallCount();

	if (panel.fall_count >= m_FieldSetting.FallCountMax()) {
		panel.fall_count = 0;
		panel.move_from = PanelPos(0, 0);

		Panel& under_panel = m_PanelContainer.GetUnderPanel(x, y);
		if (under_panel.type == Panel::TYPE_SPACE) {
			panel.state = Panel::STATE_FALLING;
			panel.move_from = PanelPos(0, 1);
			std::swap(panel, under_panel);
		}
		else if (under_panel.state == Panel::STATE_FALL_BEFORE_WAIT) {
			panel.state = Panel::STATE_FALL_BEFORE_WAIT;
			under_panel.is_chain_seed = panel.is_chain_seed;
			panel.fall_before_wait = under_panel.fall_before_wait;
		}
		else if (under_panel.state == Panel::STATE_SWAPPING) {
			// 落下時にパネルを滑り込ませて連鎖させるテクニックがある
			// 可能かどうか判定しているのでややこしいです
			const Panel& check = m_PanelContainer.GetPanel(x, y - 2);
			// 2個下がスペース
			if (check.type == Panel::TYPE_SPACE) {
				under_panel.is_chain_seed = panel.is_chain_seed;
				// 滑り込ませ連鎖できるか？
				if (under_panel.swapping_count <= (m_FieldSetting.SwappingCountMax()/2) ) {
					// STATE_FALL_AFTER_WAIT に切り替えることで
					// パネルが消せるようにする
					panel.state = Panel::STATE_FALL_AFTER_WAIT;
					panel.fall_after_wait = 0;
				}
			}
			// 2個下が地面なので地面に到達したとみなす
			else {
				panel.state = Panel::STATE_FALL_AFTER_WAIT;
				panel.fall_after_wait = 0;
			}
		}
		else {
			panel.state = Panel::STATE_FALL_AFTER_WAIT;
			panel.fall_after_wait = 0;
		}
	}
}

void GameLogic::update_PanelFallAfterWait(int x, int y, Panel& panel)
{
    panel.fall_after_wait += m_FieldSetting.IncFallAfterWait();

	Panel& under_panel = m_PanelContainer.GetUnderPanel(x, y);

	if (under_panel.state == Panel::STATE_FALL_BEFORE_WAIT) {
		panel.is_chain_seed = under_panel.is_chain_seed;
		panel.fall_before_wait = under_panel.fall_before_wait;
		panel.state = Panel::STATE_FALL_BEFORE_WAIT;
	}
	if (panel.fall_after_wait >= m_FieldSetting.FallAfterWaitMax()) {
		// パネルをリセットして
		// 通常状態へ
		panel.reset();
		panel.state = Panel::STATE_DEFAULT;
	}
}

void GameLogic::update_PanelDeleteBeforeWait(Panel& panel)
{
	panel.delete_before_wait += m_FieldSetting.IncPanelDeleteBeforeWait();
	if (panel.delete_before_wait >= m_FieldSetting.PanelDeleteBeforeWaitMax() ) {
		panel.state = Panel::STATE_DELETE;
	}
}

void GameLogic::update_PanelDelete(Panel& panel)
{
	panel.delete_wait -= m_FieldSetting.DecPanelDeleteWait();
	if (panel.delete_wait <= 0 ) {
		panel.state = Panel::STATE_DELETE_AFTER_WAIT;
		//exlib::IEventDataPtr event(new PanelDeletedEvent(mPlayerID, exlib::Vector2<int>(x, y)));
		//exlib::EventManager::instance()->queueEvent(event);
	}
}

void GameLogic::update_PanelDeleteAfterWait(int x, int y, Panel& panel)
{
	panel.delete_after_wait -= m_FieldSetting.DecPanelDeleteAfterWait();
	if (panel.delete_after_wait <= 0 ) {
		panel.reset();
		panel.state = Panel::STATE_NONE;
		panel.type  = Panel::TYPE_SPACE;
		panel.color = Panel::COLOR_NONE;
		// todo : 上のパネルに連鎖フラグを立てる
		setChainFlag(PanelPos(x, y));
	}
}

void GameLogic::setChainFlag(const PanelPos& pos)
{
	for (int y = pos.y + 1; y <= m_PanelContainer.FieldTop(); ++y) {
		Panel& panel = m_PanelContainer.GetPanel(pos.x, y);

		bool isValidType = panel.type == Panel::TYPE_PANEL;
		bool isValidState = panel.state == Panel::STATE_DEFAULT ||
			panel.state == Panel::STATE_FALL_AFTER_WAIT;

		if (isValidType && isValidState) {
			// 連鎖フラグ設定するよ！
			panel.is_chain_seed = true;
		}
		else {
			// これ以上うえのパネルは調べない
			break;
		}
	}
}

void GameLogic::updateState( int x, int y )
{
    Panel& panel = m_PanelContainer.GetPanel( x, y );
    
    // パネルとステージの状態を更新するよ！
    switch( panel.state ){
    case Panel::STATE_DEFAULT:
		update_PanelDefault(x, y, panel);
		break;
    case Panel::STATE_SWAPPING:
		update_PanelSwapping(x, y, panel);
        break;
    case Panel::STATE_FALL_BEFORE_WAIT:
		update_PanelFallBeforeWait(x, y, panel);
		m_State.ClearState(GameState::STATE_AUTO_SERIAGARI);
		break;
    break;
    case Panel::STATE_FALLING:
		update_PanelFalling(x, y, panel);
		m_State.ClearState(GameState::STATE_AUTO_SERIAGARI);
        break;
    case Panel::STATE_FALL_AFTER_WAIT:
		update_PanelFallAfterWait(x, y, panel);
		m_State.ClearState(GameState::STATE_AUTO_SERIAGARI);
		break;
    case Panel::STATE_DELETE_BEFORE_WAIT:
		update_PanelDeleteBeforeWait(panel);
		m_State.ClearState(GameState::STATE_AUTO_SERIAGARI);
		m_State.SetState(GameState::STATE_DELETE_PANEL);
        break;
    case Panel::STATE_DELETE:
		update_PanelDelete(panel);
		m_State.ClearState(GameState::STATE_AUTO_SERIAGARI);
		m_State.SetState(GameState::STATE_DELETE_PANEL);
        break;
    case Panel::STATE_DELETE_AFTER_WAIT:
		update_PanelDeleteAfterWait(x, y, panel);
		m_State.ClearState(GameState::STATE_AUTO_SERIAGARI);
		m_State.SetState(GameState::STATE_DELETE_PANEL);
        break;
    default:
        break;
    }
}



#if 0
void GameFieldLogic::updateOjyama( uint32_t id )
{
    // お邪魔パネルはIDごとにアップデートする
    OjyamaPanelPtr ojyama = mPanelContainer->getOjyamaOnStage( id );
    exlib::Vector2<short> pos = ojyama->getPos();
    Panel& panel = mPanelContainer->getPanel( pos.x, pos.y );
    assert( panel.type == Panel::TYPE_OJYAMA );

    int width = ojyama->width(), height = ojyama->height();
    int field_width = mPanelContainer->fieldWidth();
    int field_height = mPanelContainer->fieldHeight();
    // このパネルに状態もろもろセットする
    Panel setpanel = panel;

    bool is_auto_seriagari = false;
    bool is_uncompress_ojyama = false;
    
    switch( panel.state ){
    case Panel::STATE_DEFAULT:
    {
        const Panel& under_panel = isCollisionOjyama( id );
        if( under_panel.state == Panel::STATE_NONE ){
            // 下がスペース！
            setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
            setpanel.fall_before_wait = mFieldSetting.waitTimeBeforeFall();
            mPanelContainer->setOjyamaPanel( id, setpanel );
        }
        else if( under_panel.state == Panel::STATE_FALL_BEFORE_WAIT ){
            // 下が落下寸前！
            setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
            setpanel.fall_before_wait = under_panel.fall_before_wait;
            mPanelContainer->setOjyamaPanel( id, setpanel );
        }
        // 自動せり上がり有効
        is_auto_seriagari = true;
    }
    break;
    case Panel::STATE_FALL_BEFORE_WAIT:
        --setpanel.fall_before_wait;
        if( setpanel.fall_before_wait < 0 ){
            setpanel.reset();
            const Panel& under_panel = isCollisionOjyama( id );

            // パネル解凍直後は下がスペースでないこともありえるのでチェック
            if( under_panel.state == Panel::STATE_NONE ){
                setpanel.state = Panel::STATE_FALLING;
                setpanel.move_from = exlib::Vector2<short>( 0, -1 );
                mPanelContainer->downOjyamaPanel( id );
            }
            else {
                setpanel.state = Panel::STATE_FALL_AFTER_WAIT;
                setpanel.fall_after_wait = 5;
            }
        }
        mPanelContainer->setOjyamaPanel( id, setpanel );
        break;
    case Panel::STATE_FALLING:
        // 同じidのお邪魔パネルならfall_countはすべて同じなはず
        setpanel.fall_count += mFieldSetting.incFallCount();
        
        if( (setpanel.fall_count >> 16) > mFieldSetting.panelSize() ){
            const Panel& under_panel = isCollisionOjyama( id );
            setpanel.reset();
            
            if( under_panel.state == Panel::STATE_NONE ){
                // 下が全部スペースなのでそのまま
                setpanel.state = Panel::STATE_FALLING;
                setpanel.move_from = exlib::Vector2<short>( 0, -1 );
                mPanelContainer->downOjyamaPanel( id );
                std::cout << "******** PASS ********" << std::endl;
            }
            else if( under_panel.state == Panel::STATE_FALL_BEFORE_WAIT ){
                setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
                setpanel.fall_before_wait = under_panel.fall_before_wait;
            }
            else {
                setpanel.reset();
                setpanel.state = Panel::STATE_DEFAULT;
            }
        }

        mPanelContainer->setOjyamaPanel( id, setpanel );
        break;
    case Panel::STATE_FALL_AFTER_WAIT:
    {
        //assert( 0 );
        const Panel& under_panel = isCollisionOjyama( id );
        --setpanel.fall_after_wait;
        
        if( under_panel.state == Panel::STATE_FALL_BEFORE_WAIT ){
            setpanel.is_chain_seed = under_panel.is_chain_seed;
            setpanel.fall_before_wait = under_panel.fall_before_wait;
            setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
        }
        if( setpanel.fall_after_wait < 0 ){
            setpanel.reset();
            setpanel.state = Panel::STATE_DEFAULT;
        }
        mPanelContainer->setOjyamaPanel( id, setpanel );

        // 自動せり上がり有効
        is_auto_seriagari = true;
    }
    break;
    case Panel::STATE_UNCOMPRESS_AFTER_WAIT:
        // 解凍が次で終了するなら
        if( panel.delete_after_wait - 1 < 0 ){
            // お邪魔のラインが0なら
            // フィールド上のお邪魔データから除外しよう
            if( ojyama->updateDecompressed() ){
                mPanelContainer->eraseOjyamaOnStage( id );
            }
        }
        /* FALLTHROUGH */
    case Panel::STATE_UNCOMPRESS_BEFORE_WAIT:
    case Panel::STATE_UNCOMPRESS:

        // パネル解凍中
        is_uncompress_ojyama = true;
        
        // お邪魔パネルの状態更新
        for( int i = pos.y;
             i >= 0 && i < field_height && i > pos.y - height;
             --i ){
            for( int j = pos.x;
                 j >= 1 && j <= field_width && j <= width;
                 ++j ){
                Panel& dstpanel = mPanelContainer->getPanel( j, i );

                if( dstpanel.state == Panel::STATE_UNCOMPRESS_BEFORE_WAIT ){
                    --dstpanel.delete_before_wait;
                    std::cout << "uncompress_before_wait : " << dstpanel.delete_before_wait << std::endl;
                    if( dstpanel.delete_before_wait < 0 ){
                        dstpanel.state = Panel::STATE_UNCOMPRESS;
                    }
                }
                else if( dstpanel.state == Panel::STATE_UNCOMPRESS ){
                    --dstpanel.delete_wait;
                    std::cout << "uncompress_wait : " << dstpanel.delete_wait << std::endl;
                    if( dstpanel.delete_wait < 0 ){
                        dstpanel.state = Panel::STATE_UNCOMPRESS_AFTER_WAIT;
                    }
                }
                else if( dstpanel.state == Panel::STATE_UNCOMPRESS_AFTER_WAIT ){
                    --dstpanel.delete_after_wait;
                    std::cout << "uncompress_after_wait : " << dstpanel.delete_after_wait << std::endl;
                    if( dstpanel.delete_after_wait < 0 ){
                        dstpanel.type  = dstpanel.is_uncompress ? Panel::TYPE_PANEL : Panel::TYPE_OJYAMA;
                        dstpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
                    }
                }
                else {
                    assert( 0 );
                }
            }
        }
    default:
        // 自動せり上がり有効
        is_auto_seriagari = true;
        break;
    }

    // お邪魔パネルも普通のパネルと同様に
    // 一つでも自動せり上がりできない条件なら
    // そのフレームは自動せり上がりしないよ
    if( !is_auto_seriagari ){
        mState.clearState( GameState::STATE_AUTO_SERIAGARI );
    }
    if( is_uncompress_ojyama ){
        mState.setState( GameState::STATE_UNCOMPRESS_OJYAMA );
    }
}
#endif 

#if 0
Panel GameFieldLogic::isCollisionOjyama( uint32_t id )
{
    int field_width = mPanelContainer->fieldWidth();
    
    OjyamaPanelPtr ojyama = mPanelContainer->getOjyamaOnStage( id );
    exlib::Vector2<short> pos = ojyama->getPos();

    Panel result;
    result.type = Panel::TYPE_SPACE;
    result.state = Panel::STATE_NONE;
    for( int j = pos.x;
         j >= 1 && j <= field_width && j <= ojyama->width();
         ++j ){
        Panel& under_panel = mPanelContainer->getPanel( j, pos.y + 1 );

        if( under_panel.state == Panel::STATE_FALL_BEFORE_WAIT ){
            result = under_panel;
        }
        else if( under_panel.type != Panel::TYPE_SPACE ){
            result = under_panel;
            break;
        }
    }
	return result;
}
#endif

