
#ifndef   HOGEPON_PANEL_HPP
#define   HOGEPON_PANEL_HPP

#include <cstdint>

struct PanelPos
{
	int x;
	int y;

	PanelPos() : x(0), y(0) {}
	PanelPos(int ax, int ay) : x(ax), y(ay) {}
};

struct Panel
{
public:

    enum Type
    {
        TYPE_SPACE = 0,
        TYPE_PANEL,
        TYPE_OJYAMA,
		TYPE_WALL
    };
        
    enum State
    {
        STATE_DEFAULT = 0,
        STATE_SWAPPING,
        STATE_FALL_BEFORE_WAIT,
        STATE_FALLING,
        STATE_FALL_AFTER_WAIT,
        STATE_DELETE_BEFORE_WAIT,
        STATE_DELETE,
        STATE_DELETE_AFTER_WAIT,
        STATE_UNCOMPRESS_BEFORE_WAIT,
        STATE_UNCOMPRESS,
        STATE_UNCOMPRESS_AFTER_WAIT,
        STATE_NONE,
    };

    enum Color
    {
        COLOR_RED = 0,
        COLOR_GREEN,
        COLOR_WATER_BLUE,
        COLOR_YELLOW,
        COLOR_PURPLE,
        COLOR_BLUE,
        COLOR_BIKKURI,
        COLOR_NONE,
        COLOR_NUM
    };

    Panel();
	~Panel() noexcept;

    // パネルのカウンタをリセットする
    // ただし 'Type' 'State' 'Color' は変更しない
    void reset();
	
    // パネル消去フラグなど、一時的に使用するためのフラグをクリアする
	void ClearMark();

    // この値を現在のパネルの位置に加えると
    // 交換前のパネルの位置に戻れる
    PanelPos move_from;

    // タイマー変数
    // uint32_t 型のカウンタは16.16の固定小数値として用いる
	int		delete_before_wait;			// 消去前ウェイト
    int		delete_wait;				// 消去中ウェイト
    int		delete_after_wait;			// 消去後ウェイト
	uint32_t swapping_count;			// 交換時カウンタ
    int		fall_before_wait;			// パネル落下前ウェイト
    uint32_t fall_count;				// 落下時カウンタ
    int		fall_after_wait;			// 落下後ウェイト
    bool is_chain_seed;					// 連鎖フラグ
	bool is_mark_delete;				// パネル消去フラグ（消去チェック時に使用する）
    bool is_uncompress;					// 解凍されてパネルになるか
    
    uint32_t ojyama_id;
    Type  type;
    State state;
    Color color;
};


#endif   //   HOGEPON_PANEL_HPP

