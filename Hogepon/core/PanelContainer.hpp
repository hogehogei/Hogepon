
#ifndef   HOGEPON_PANEL_CONTAINER_HPP
#define   HOGEPON_PANEL_CONTAINER_HPP

#include "Siv3D.hpp"
#include "core/PanelCreator.hpp"
#include "core/OjyamaPanel.hpp"
#include "core/OjyamaCreator.hpp"

class PanelContainer
{
public:

    using OjyamaPanelVec = std::vector<OjyamaPanel>;

public:


    PanelContainer() = default;
    ~PanelContainer() noexcept = default;

	void Initialize(int w, int h, uint32_t seed);

    Panel& GetPanel( int x, int y );
	const Panel& GetPanel(int x, int y) const;
	Panel& GetUnderPanel(int x, int y);
    void SetPanel(int x, int y, const Panel& panel);

    PanelCreator& GetPanelCreator();

    OjyamaPanel GetOjyamaPanel(uint32_t ojyama_id);
    OjyamaPanelVec GetOjyamaPanelListOnField();


	bool Is_InFieldCursor(int x, int y) const;
	bool Is_InField(int x, int y) const;

    void ClearOjyamaUpdated();
    void FallOjyamaLine(bool dequeue_enable);
    void OjyamaAppend(int chain_count, int doujikeshi_count);

    int FieldNewOjyamaLine() const { return m_RealHeight - 1; }
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

    // お邪魔パネル管理
    OjyamaCreator m_OjyamaCreator;

	int m_Width;
	int m_Height;
	int m_RealWidth;
	int m_RealHeight;
};



#endif   // HOGEPON_PANEL_CONTAINER_HPP


