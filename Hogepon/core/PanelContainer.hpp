
#ifndef   HOGEPON_PANEL_CONTAINER_HPP
#define   HOGEPON_PANEL_CONTAINER_HPP

#include "Siv3D.hpp"
#include "core/PanelCreator.hpp"


class PanelContainer
{
public:

    PanelContainer();
	~PanelContainer() noexcept;

	void Initialize(int w, int h, uint32_t seed);

    Panel& GetPanel( int x, int y );
	const Panel& GetPanel(int x, int y) const;
	Panel& GetUnderPanel(int x, int y);
	void SetPanel(int x, int y, const Panel& panel);

	bool Is_InFieldCursor(int x, int y) const;
	bool Is_InField(int x, int y) const;

    /**
     * @brief ステージの幅
     */
    int Width() const { return m_Width; }
    /**
     * @brief ステージの高さ
     */
    int Height() const { return m_Height; }

	int FieldTop() const { return m_Height; }
	int FieldBottom() const { return 1; }
	int FieldNextLine() const { return 0; }
	int FieldLeft() const { return 0; }
	int FieldRight() const { return m_Width - 1; }

	void NewLine();
        
private:

	void initializeField();								// フィールド初期化

    // panel のクラスを持たせる
    s3d::Array<Panel> m_Panels;

	// パネル製造機
	PanelCreator m_PanelCreator;

	int m_Width;
	int m_Height;
	int m_RealWidth;
	int m_RealHeight;

    // お邪魔パネルを管理
    //OjyamaQueue mOjyamaQueue;
    //OjyamaList  mOjyamaOnStage;
};



#endif   // HOGEPON_PANEL_CONTAINER_HPP


