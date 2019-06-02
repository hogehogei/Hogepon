
#include <random>
#include <Siv3D.hpp>
#include "core/GameLogic.hpp"
#include "core/OjyamaPanel.hpp"

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
    m_PanelContainer.ClearOjyamaUpdated();

	// 入力に対する処理
	processInput();
	// フィールドがピンチか調べる
	check_IsPlayerPinch();
    // お邪魔パネルを降らせる
    fallOjyamaLine();

    //
    // パネルの状態をupdate
    // 下から順番に調べていく
    // なぜ下から調べるかというと、パネルが落ちるときに
    // 上から調べるとずれが生じるから
    // 
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldNewOjyamaLine(); ++y ) {
        for( int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x ){
            Panel& panel = m_PanelContainer.GetPanel( x, y );

            // 連鎖が終了したかチェックします
            if( panel.is_chain_seed ){
                isEndChain = false;
            }

            if (panel.type == Panel::TYPE_OJYAMA) {
                updateOjyamaPanel(x, y);
            }
            else {
                // パネルの状態を更新
                updatePanel(x, y);
            }
        }
    }

	doSeriagari();
	deletePanel();
    uncompressOjyama();
	judgeGameOver();

    // 連鎖終了ならチェインカウントを戻そう
    if( isEndChain ){
		m_State.EndChain();
    }
}

void GameLogic::fallOjyamaLine()
{
    bool newojyama_fall = false;

    if (canSeriagari()) {
        newojyama_fall = true;
    }
   
    m_PanelContainer.FallOjyamaLine(newojyama_fall);
}

void GameLogic::clearPanelMark()
{
	for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldNewOjyamaLine(); ++y) {
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

    // テスト用
    // お邪魔パネルを落下させる
    if (s3d::KeyO.down()) {
        m_PanelContainer.OjyamaAppend(3, 0);
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
	left.Reset();
	right.Reset();
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
				if (y == line_danger) {
					m_State.SetState(GameState::STATE_DANGER);
				}
			}
		}
	}
}

bool GameLogic::canSeriagari()
{
	bool can_seriagari = true;

    // 一番上の列にパネルがある場合は、せり上がりできない
    if (m_State.IsDanger()) {
        can_seriagari = false;
    }

	// パネルの状態により、せり上がりできる状態か調べる
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
            // [TODO] ここの判定条件は確認すること
            // お邪魔消去中もゲームオーバーにならないが、それが考慮されていない？
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
				panel.delete_before_wait = m_FieldSetting.DeleteBeforeWait();
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

void GameLogic::uncompressOjyama()
{
    for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
        for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
            // 消去パネル周辺のお邪魔パネルに解凍マークをつける
            if (m_PanelContainer.GetPanel(x, y).is_mark_delete) {
                markUncompressOjyama(x - 1, y);
                markUncompressOjyama(x + 1, y);
                markUncompressOjyama(x, y - 1);
                markUncompressOjyama(x, y + 1);
            }
        }
    }

    // 解凍マークがついているお邪魔パネルは最下段だけ解凍
    auto ojyama_list = m_PanelContainer.GetOjyamaPanelListOnField();
    for (auto itr = ojyama_list.begin(); itr != ojyama_list.end(); ++itr) {
        const Panel& basepanel = itr->GetBasePanel();
        if (basepanel.is_mark_uncompress) {
            itr->UncompressBottomLine();
        }
    }

    setUncompressOjyamaTimer();
}

void GameLogic::markUncompressOjyama(int x, int y)
{
    // フィールド外だったら何もしない
    if (!m_PanelContainer.Is_InField(x, y)) {
        return;
    }

    Panel& panel = m_PanelContainer.GetPanel(x, y);
    // すでにマーク済みでなく、落下中、消去中でなければ消す
    // [TODO] お邪魔パネルも！マークとそうでないものは同時に消えない
    // お邪魔パネルのタイプを持たせるしかない
    if (panel.is_mark_uncompress == false &&
        panel.type == Panel::TYPE_OJYAMA && 
        panel.state == Panel::STATE_DEFAULT) {

        panel.is_mark_uncompress = true;

        // 周囲のお邪魔パネルも再帰処理で判定
        markUncompressOjyama(x - 1, y);
        markUncompressOjyama(x + 1, y);
        markUncompressOjyama(x, y - 1);
        markUncompressOjyama(x, y + 1);
    }
}

void GameLogic::setUncompressOjyamaTimer()
{
    // 消去対象のパネルは is_mark_be_panel == true となるので
    // 最下段からお邪魔消去タイマーをセットする
    int delete_panel_num = countDeleteMarkPanel();
    int uncompress_panel_num = countDeleteMarkUncompress();
    int uncompress_wait_per_panel = m_FieldSetting.WaitTimePanelDel();
    int uncompress_time_max = uncompress_wait_per_panel * uncompress_panel_num;
    int uncompress_cnt = 0;

    bool is_chain = false;

    for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
        for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
            Panel& panel = m_PanelContainer.GetPanel(x, y);

            if (panel.is_mark_uncompress){
                ++uncompress_cnt;
                
                panel.state = Panel::STATE_UNCOMPRESS_BEFORE_WAIT;
                panel.uncompress_before_wait = m_FieldSetting.UncompressBeforeWait() + (delete_panel_num * uncompress_wait_per_panel);
                panel.uncompress_wait = uncompress_wait_per_panel * uncompress_cnt;
                panel.uncompress_after_wait = uncompress_time_max - panel.uncompress_wait + 1;
            }
        }
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

int GameLogic::countDeleteMarkUncompress()
{
    int uncompress_panel_count = 0;
    for (int y = m_PanelContainer.FieldBottom(); y <= m_PanelContainer.FieldTop(); ++y) {
        for (int x = m_PanelContainer.FieldLeft(); x <= m_PanelContainer.FieldRight(); ++x) {
            if (m_PanelContainer.GetPanel(x, y).is_mark_uncompress) {
                ++uncompress_panel_count;
            }
        }
    }

    return uncompress_panel_count;
}

void GameLogic::updatePanel(int x, int y)
{
    Panel& panel = m_PanelContainer.GetPanel(x, y);

    // パネルとステージの状態を更新するよ！
    switch (panel.state) {
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

void GameLogic::update_PanelDefault(int x, int y, Panel& panel)
{
	const Panel& under_panel = m_PanelContainer.GetUnderPanel(x, y);
    // 下の空白が交換中、もしくは消去後ウェイト以外なら落下
	if (under_panel.type == Panel::TYPE_SPACE &&
		under_panel.state != Panel::STATE_SWAPPING &&
        under_panel.state != Panel::STATE_DELETE_AFTER_WAIT) {
		panel.fall_before_wait = 0;
		panel.state = Panel::STATE_FALL_BEFORE_WAIT;
	}
	else if (under_panel.state == Panel::STATE_FALL_BEFORE_WAIT) {
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
			else {
				panel.Reset();
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
        // 下の空白が消去後ウェイト以外なら落下
		if (under_panel.type == Panel::TYPE_SPACE && under_panel.state != Panel::STATE_DELETE_AFTER_WAIT) {
			panel.fall_before_wait = 0;
			panel.state = Panel::STATE_FALLING;
			panel.move_from = PanelPos(0, 1);
			std::swap(panel, under_panel);
		}
		else {
			panel.state = Panel::STATE_FALL_AFTER_WAIT;
			panel.fall_after_wait = 1;
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
        // 下の空白が消去後ウェイト以外なら落下
		if (under_panel.type == Panel::TYPE_SPACE && under_panel.state != Panel::STATE_DELETE_AFTER_WAIT) {
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
				if (under_panel.swapping_count == 0 ) {
					// STATE_FALL_AFTER_WAIT に切り替えることで
					// パネルが消せるようにする
					panel.state = Panel::STATE_FALL_AFTER_WAIT;
					panel.fall_after_wait = 1;
				}
			}
			// 2個下が地面なので地面に到達したとみなす
			else {
				panel.state = Panel::STATE_FALL_AFTER_WAIT;
				panel.fall_after_wait = 1;
			}
		}
		else {
			panel.state = Panel::STATE_FALL_AFTER_WAIT;
			panel.fall_after_wait = 1;
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
	else if (panel.fall_after_wait >= m_FieldSetting.FallAfterWaitMax()) {
		// パネルをリセットして
		// 通常状態へ
		panel.Reset();
		panel.state = Panel::STATE_DEFAULT;
	}
}

void GameLogic::update_PanelDeleteBeforeWait(Panel& panel)
{
	panel.delete_before_wait -= m_FieldSetting.DecPanelDeleteBeforeWait();
	if (panel.delete_before_wait <= 0 ) {
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
    // 消去したパネルの空白に対して、パネルを交換して滑り込ませて連鎖する技をスムーズにするため
    // パネルをスペースに設定しなおして、表示はせず交換できる状態にしておく
    panel.type  = Panel::TYPE_SPACE;
    panel.color = Panel::COLOR_NONE;
	panel.delete_after_wait -= m_FieldSetting.DecPanelDeleteAfterWait();

	if (panel.delete_after_wait <= 0 ) {
		panel.Reset();
		panel.state = Panel::STATE_NONE;
		// 上のパネルに連鎖フラグを立てる
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

void GameLogic::updateOjyamaPanel( int x, int y )
{
    const Panel& ojyama_panel = m_PanelContainer.GetPanel(x, y);

    switch( ojyama_panel.state ){
    case Panel::STATE_DEFAULT:
        update_OjyamaDefault(x, y);
        break;

    case Panel::STATE_FALL_BEFORE_WAIT:
        update_OjyamaFallBeforeWait(x, y);
        break;

    case Panel::STATE_FALLING:
        update_OjyamaFalling(x, y);
        break;

    case Panel::STATE_FALL_AFTER_WAIT:
        update_OjyamaFallAfterWait(x, y);
        break;

    case Panel::STATE_UNCOMPRESS_BEFORE_WAIT:
        update_OjyamaUncompressBeforeWait(x, y);
        break;

    case Panel::STATE_UNCOMPRESS:
        update_OjyamaUncompress(x, y);
        break;

    case Panel::STATE_UNCOMPRESS_AFTER_WAIT:
        update_OjyamaUncompressAfterWait(x, y);
        break;

    default:
        break;
    }
}

void GameLogic::update_OjyamaDefault(int x, int y)
{
    const Panel& panel = m_PanelContainer.GetPanel(x, y);
    // 更新済みならば更新しない
    if (panel.ojyama->is_updated) {
        return;
    }

    // お邪魔パネルはIDごとにアップデートする
    OjyamaPanel ojyama_panel = m_PanelContainer.GetOjyamaPanel(x, y);
    // このパネルに状態もろもろセットする
    Panel setpanel = panel;
    Panel under_panel;

    // 下の空白が交換中、もしくは消去後ウェイト以外なら落下
    if (ojyama_panel.CanFallOjyamaPanel()) {
        setpanel.fall_before_wait = 0;
        setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
    }
    else if (ojyama_panel.IsExistUnderOjyama(Panel::STATE_FALL_BEFORE_WAIT, &under_panel)) {
        setpanel.fall_before_wait = under_panel.fall_before_wait;
        setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
    }

    ojyama_panel.SetOjyamaPanel(setpanel);
    // 更新済みマークをつける
    ojyama_panel.SetUpdated();
}

void GameLogic::update_OjyamaFallBeforeWait(int x, int y)
{
    const Panel& panel = m_PanelContainer.GetPanel(x, y);
    // 更新済みならば更新しない
    if (panel.ojyama->is_updated) {
        return;
    }

    // お邪魔パネルはIDごとにアップデートする
    OjyamaPanel ojyama_panel = m_PanelContainer.GetOjyamaPanel(x, y);
    // このパネルに状態もろもろセットする
    Panel setpanel = panel;
    Panel under_panel;

    setpanel.fall_before_wait += m_FieldSetting.IncFallBeforeWait();

    // まだ落下待ちのパネルがある場合は、それが落ち始めるまで現状維持
    if (ojyama_panel.IsExistUnderOjyama(Panel::STATE_FALL_BEFORE_WAIT, &under_panel)) {
        setpanel.fall_before_wait = under_panel.fall_before_wait;
        setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
    }
    // 落下待ちのパネルなし。かつ、落下ウェイトが待ち時間を越えたら
    else if (setpanel.fall_before_wait >= m_FieldSetting.FallBeforeWaitMax()) {
        // 下の空白が消去後ウェイト以外なら落下
        if (ojyama_panel.CanFallOjyamaPanel()) {
            ojyama_panel.Fall();
            // 更新済みマークをつける
            ojyama_panel.SetUpdated();
        }
        else {
            setpanel.state = Panel::STATE_FALL_AFTER_WAIT;
            setpanel.fall_after_wait = 1;
        }
    }

    if (!ojyama_panel.IsUpdated()) {
        ojyama_panel.SetOjyamaPanel(setpanel);
        // 更新済みマークをつける
        ojyama_panel.SetUpdated();
    }
}

void GameLogic::update_OjyamaFalling(int x, int y)
{
    const Panel& panel = m_PanelContainer.GetPanel(x, y);
    // 更新済みならば更新しない
    if (panel.ojyama->is_updated) {
        return;
    }

    // お邪魔パネルはIDごとにアップデートする
    OjyamaPanel ojyama_panel = m_PanelContainer.GetOjyamaPanel(x, y);
    // このパネルに状態もろもろセットする
    Panel setpanel = panel;
    Panel under_panel;

    setpanel.fall_count += m_FieldSetting.IncFallCount();

    if (setpanel.fall_count >= m_FieldSetting.FallCountMax()) {
        setpanel.fall_count = 0;
        setpanel.move_from = PanelPos(0, 0);

        // 下の空白が消去後ウェイト以外なら落下
        if (ojyama_panel.CanFallOjyamaPanel()) {
            ojyama_panel.Fall();
            // 更新済みマークをつける
            ojyama_panel.SetUpdated();
        }
        else if (ojyama_panel.IsExistUnderOjyama(Panel::STATE_FALL_BEFORE_WAIT, &under_panel)) {
            setpanel.fall_before_wait = under_panel.fall_before_wait;
            setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
        }
        else {
            setpanel.Reset();
            setpanel.state = Panel::STATE_DEFAULT;
        }
    }

    if (!ojyama_panel.IsUpdated()) {
        ojyama_panel.SetOjyamaPanel(setpanel);
        // 更新済みマークをつける
        ojyama_panel.SetUpdated();
    }
}

void GameLogic::update_OjyamaFallAfterWait(int x, int y)
{
    const Panel& panel = m_PanelContainer.GetPanel(x, y);
    // 更新済みならば更新しない
    if (panel.ojyama->is_updated) {
        return;
    }

    // お邪魔パネルはIDごとにアップデートする
    OjyamaPanel ojyama_panel = m_PanelContainer.GetOjyamaPanel(x, y);
    // このパネルに状態もろもろセットする
    Panel setpanel = panel;
    Panel under_panel;

    setpanel.fall_after_wait += m_FieldSetting.IncFallAfterWait();
    if (ojyama_panel.IsExistUnderOjyama(Panel::STATE_FALL_BEFORE_WAIT, &under_panel)) {
        setpanel.fall_before_wait = under_panel.fall_before_wait;
        setpanel.state = Panel::STATE_FALL_BEFORE_WAIT;
    }
    else if (setpanel.fall_after_wait >= m_FieldSetting.FallAfterWaitMax()) {
        // パネルをリセットして
        // 通常状態へ
        setpanel.Reset();
        setpanel.state = Panel::STATE_DEFAULT;
    }

    ojyama_panel.SetOjyamaPanel(setpanel);
    // 更新済みマークをつける
    ojyama_panel.SetUpdated();
}

void GameLogic::update_OjyamaUncompressBeforeWait(int x, int y)
{
    // 消去中は個別にカウンタを更新する
    Panel& panel = m_PanelContainer.GetPanel(x, y);
    panel.uncompress_before_wait -= m_FieldSetting.DecOjyamaPanelDeleteBeforeWait();

    if (panel.uncompress_before_wait <= 0 ) {
        panel.state = Panel::STATE_UNCOMPRESS;
    }
}

void GameLogic::update_OjyamaUncompress(int x, int y)
{
    // 消去中は個別にカウンタを更新する
    Panel& panel = m_PanelContainer.GetPanel(x, y);

    panel.uncompress_wait -= m_FieldSetting.DecOjyamaPanelDeleteWait();
    if (panel.uncompress_wait <= 0) {
        panel.state = Panel::STATE_UNCOMPRESS_AFTER_WAIT;
    }
}

void GameLogic::update_OjyamaUncompressAfterWait(int x, int y)
{
    // 消去中は個別にカウンタを更新する
    Panel& panel = m_PanelContainer.GetPanel(x, y);
    // お邪魔パネルはIDごとにアップデートする
    OjyamaPanel ojyama_panel = m_PanelContainer.GetOjyamaPanel(x, y);

    panel.uncompress_after_wait -= m_FieldSetting.DecOjyamaPanelDeleteAfterWait();

    if (panel.uncompress_after_wait <= 0) {

        panel.state = Panel::STATE_FALL_BEFORE_WAIT;
        // お邪魔パネルの最下段ならパネルになる。それ以外ならお邪魔パネルのまま
        if (panel.is_mark_be_panel) {
            panel.Reset();
            panel.type = Panel::TYPE_PANEL;
            // お邪魔パネル情報をクリア
            panel.ojyama.reset();
            // パネル消去後は連鎖判定あり
            panel.is_chain_seed = true;
        }
        else {
            // panel.Reset();
            panel.type  = Panel::TYPE_OJYAMA;
        }
    }
}


