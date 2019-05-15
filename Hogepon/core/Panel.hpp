
#ifndef   HOGEPON_PANEL_HPP
#define   HOGEPON_PANEL_HPP

#include <cstdint>
#include <memory>

struct PanelPos
{
	int x;
	int y;

	PanelPos() : x(0), y(0) {}
	PanelPos(int ax, int ay) : x(ax), y(ay) {}
};

struct Ojyama
{
    uint32_t id;                 // オジャマID 同一であれば、同じオジャマパネルと判断する
    uint8_t  width;              // オジャマパネルの幅
    uint8_t  height;             // オジャマパネルの高さ
    bool     is_updated;         // 更新済み？

    Ojyama() :
        id(0),
        width(0),
        height(0),
        is_updated( false )
    {}
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
    void Reset();
	
    // パネル消去フラグなど、一時的に使用するためのフラグをクリアする
	void ClearMark();



    // 変数

    // この値を現在のパネルの位置に加えると
    // 交換前のパネルの位置に戻れる
    PanelPos move_from;

    // タイマー変数
    // uint32_t 型のカウンタは16.16の固定小数値として用いる
	int      delete_before_wait;		// 消去前ウェイト
    int		 delete_wait;				// 消去中ウェイト
    int		 delete_after_wait;			// 消去後ウェイト
	uint32_t swapping_count;			// 交換時カウンタ
    int		 fall_before_wait;			// パネル落下前ウェイト
    uint32_t fall_count;				// 落下時カウンタ
    int		 fall_after_wait;			// 落下後ウェイト
    bool     is_chain_seed;				// 連鎖フラグ
	bool     is_mark_delete;			// パネル消去フラグ（消去チェック時に使用する）
    bool     is_mark_uncompress;

    // オジャマパネル関連
    bool     is_mark_be_panel;          // 解凍されてパネルになるか？
    PanelPos ojyama_basepos_from;       // お邪魔パネルのベース位置からの距離
    std::shared_ptr<Ojyama> ojyama;     // オジャマパネルへの情報

    Type  type;
    State state;
    Color color;
};


#endif   //   HOGEPON_PANEL_HPP

