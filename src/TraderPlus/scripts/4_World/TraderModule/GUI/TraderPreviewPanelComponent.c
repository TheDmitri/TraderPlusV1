class TraderPreviewPanelComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("trader-preview-panel-component", TraderPreviewPanelComponent);

    static ref ScriptInvoker Event_OnBuyRequested = new ScriptInvoker();
    static ref ScriptInvoker Event_OnSellRequested = new ScriptInvoker();
    static ref ScriptInvoker Event_OnQuantityChanged = new ScriptInvoker();
    static ref ScriptInvoker Event_OnPreviewSwitchRequested = new ScriptInvoker();

    // Bound properties
    string buyPriceText;
    string sellPriceText;
    EntityAI preview;

    // Widgets loaded via AutoLoadWidgets
    Widget BuyPanelWidget;
    Widget SellPanelWidget;
    Widget GridInsurance;
    Widget GridCenter;
    Widget PanelWidgetPreview;
    MultilineTextWidget ItemDescription;
    RichTextWidget InformationText;
    TextWidget InsuranceTextOuput;
    TextWidget CargoCount;
    TextWidget SizeCount;
    ButtonWidget BttnKit;
    ButtonWidget BttnReset;
    ButtonWidget BttnRemove;
    ButtonWidget BttnPlayerPreview;
    ImageWidget ImagePlayerPreview;
    XComboBoxWidget QuantityMultiplier;
    CheckBoxWidget InsuranceCheckBox;
    ItemPreviewWidget MainItemPreview;

    // Internal state
    private int m_PreviewID;
    private EntityAI m_previewItem;
    private PlayerBase m_previewPlayer;
    private int m_MainItemPreviewRotationX;
    private int m_MainItemPreviewRotationY;
    private vector m_MainItemPreviewOrientation;
    private int m_MainItemPreviewScale;
    private vector m_CharacterOrientation;
    private int m_QtyMultiplier;
    private ref TraderPlusProduct m_CurrentProduct;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "trader-preview-panel-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/TraderPreviewPanelComponent.layout";
    }

    override void OnInit()
    {
        m_PreviewID = TRADERPLUS_MAINITEMPREVIEW;
        m_CharacterOrientation = "0 0 0";
        m_QtyMultiplier = 1;
        m_CurrentProduct = new TraderPlusProduct;

        if (BttnKit)
        {
            BttnKit.Show(false);
        }
        if (BttnReset)
        {
            BttnReset.Show(false);
        }
        if (BttnRemove)
        {
            BttnRemove.Show(false);
        }
        if (GridInsurance)
        {
            GridInsurance.Show(false);
        }

        buyPriceText = "#tpm_buy ";
        sellPriceText = "#tpm_sell ";
        MarkForCheck();
    }

    override void OnDestroy()
    {
        CleanupPreview();
    }

    void CleanupPreview()
    {
        if (m_previewItem)
        {
            GetGame().ObjectDelete(m_previewItem);
            m_previewItem = null;
        }
        if (m_previewPlayer)
        {
            GetGame().ObjectDelete(m_previewPlayer);
            m_previewPlayer = null;
        }
    }

    // --- Public API for parent to call ---

    TraderPlusProduct GetCurrentProduct()
    {
        return m_CurrentProduct;
    }

    int GetQtyMultiplier()
    {
        return m_QtyMultiplier;
    }

    void ResetMultiplier()
    {
        m_QtyMultiplier = 1;
        if (QuantityMultiplier)
        {
            QuantityMultiplier.ClearAll();
            QuantityMultiplier.AddItem("1");
        }
    }

    void ShowBuyPanel()
    {
        if (BuyPanelWidget)
        {
            BuyPanelWidget.Show(true);
        }
        if (SellPanelWidget)
        {
            SellPanelWidget.Show(false);
        }
    }

    void ShowSellPanel()
    {
        if (BuyPanelWidget)
        {
            BuyPanelWidget.Show(false);
        }
        if (SellPanelWidget)
        {
            SellPanelWidget.Show(true);
        }
    }

    void SetInformationText(string text)
    {
        if (InformationText)
        {
            InformationText.SetText(text);
        }
    }

    void SetDescriptionText(string text)
    {
        if (ItemDescription)
        {
            ItemDescription.SetText(text);
        }
    }

    void SetBuyPriceText(string text)
    {
        buyPriceText = text;
        MarkForCheck();
    }

    void SetSellPriceText(string text)
    {
        sellPriceText = text;
        MarkForCheck();
    }

    void SetInsuranceVisible(bool visible)
    {
        if (GridInsurance)
        {
            GridInsurance.Show(visible);
        }
    }

    void SetInsuranceText(string text)
    {
        if (InsuranceTextOuput)
        {
            InsuranceTextOuput.SetText(text);
        }
    }

    bool IsInsuranceChecked()
    {
        if (InsuranceCheckBox)
        {
            return InsuranceCheckBox.IsChecked();
        }
        return false;
    }

    void SetCargoCount(string text)
    {
        if (CargoCount)
        {
            CargoCount.SetText(text);
        }
    }

    void SetSizeCount(string text)
    {
        if (SizeCount)
        {
            SizeCount.SetText(text);
        }
    }

    void SetCurrentProduct(TraderPlusProduct product)
    {
        m_CurrentProduct = product;
    }

    // --- Preview ---

    void UpdateItemPreview(string itemType)
    {
        if (m_PreviewID == TRADERPLUS_PLAYERPREVIEW)
        {
            return;
        }

        string tempstr = itemType;
        tempstr.ToLower();
        if (BttnKit)
        {
            if (tempstr.Contains("kit"))
            {
                BttnKit.Show(true);
            }
            else
            {
                BttnKit.Show(false);
            }
        }

        if (itemType.Contains(GetTraderPlusConfigClient().LicenceKeyWord))
        {
            itemType = "Paper";
        }

        if (m_previewItem)
        {
            GetGame().ObjectDelete(m_previewItem);
            m_previewItem = null;
        }

        m_previewItem = EntityAI.Cast(GetGame().CreateObjectEx(itemType, vector.Zero, ECE_LOCAL | ECE_NOLIFETIME));
        if (!m_previewItem)
        {
            return;
        }

        preview = m_previewItem;
        MarkForCheck();
    }

    void ResetPreview()
    {
        if (m_previewItem)
        {
            GetGame().ObjectDelete(m_previewItem);
            m_previewItem = null;
        }
        preview = null;
        buyPriceText = "#tpm_buy ";
        sellPriceText = "#tpm_sell ";
        SetInformationText("");
        MarkForCheck();
    }

    void InitPlayerPreview()
    {
        vector dir = GetGame().GetCurrentCameraDirection();
        dir.Normalize();
        vector pos = GetGame().GetCurrentCameraPosition() - dir * 0.5;
        pos[1] = GetGame().GetPlayer().GetPosition()[1];

        if (m_previewPlayer)
        {
            GetGame().ObjectDelete(m_previewPlayer);
        }
        m_previewPlayer = PlayerBase.Cast(GetGame().CreateObjectEx(GetGame().GetPlayer().GetType(), pos, ECE_LOCAL | ECE_NOLIFETIME));

        if (m_previewPlayer)
        {
            m_previewPlayer.m_IsStashDisable = true;
            for (int i = 0; i < TraderPlusHelper.playerAttachments.Count(); i++)
            {
                EntityAI ent = GetGame().GetPlayer().FindAttachmentBySlotName(TraderPlusHelper.playerAttachments[i]);
                if (ent)
                {
                    if (TraderPlusHelper.playerAttachments[i] == "Hand")
                    {
                        m_previewPlayer.GetHumanInventory().CreateInHands(ent.GetType());
                        continue;
                    }
                    int slotId = InventorySlots.GetSlotIdFromString(TraderPlusHelper.playerAttachments[i]);
                    m_previewPlayer.GetInventory().CreateAttachmentEx(ent.GetType(), slotId);
                }
            }
        }
    }

    // --- Button handlers ---

    void OnBuyClicked(ClickEventArgs args)
    {
        Event_OnBuyRequested.Invoke();
    }

    void OnSellClicked(ClickEventArgs args)
    {
        Event_OnSellRequested.Invoke();
    }

    // --- Quantity multiplier ---

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == QuantityMultiplier)
        {
            if (m_CurrentProduct.Price == -1)
            {
                return true;
            }
            m_QtyMultiplier = QuantityMultiplier.GetCurrentItem() + 1;
            Event_OnQuantityChanged.Invoke(m_QtyMultiplier);
            return true;
        }

        if (w == InsuranceCheckBox)
        {
            Event_OnQuantityChanged.Invoke(m_QtyMultiplier);
            return true;
        }

        if (w == BttnPlayerPreview)
        {
            Event_OnPreviewSwitchRequested.Invoke();
            return true;
        }

        return false;
    }

    // --- Mouse drag for preview rotation ---

    override bool OnMouseButtonDown(Widget w, int x, int y, int button)
    {
        if (w == MainItemPreview)
        {
            GetGame().GetDragQueue().Call(this, "UpdateRotation");
            g_Game.GetMousePos(m_MainItemPreviewRotationX, m_MainItemPreviewRotationY);
            return true;
        }
        return false;
    }

    void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging)
    {
        if (!MainItemPreview)
        {
            return;
        }
        vector orientation = m_MainItemPreviewOrientation;
        orientation[0] = orientation[0] + (m_MainItemPreviewRotationY - mouse_y);
        orientation[1] = orientation[1] - (m_MainItemPreviewRotationX - mouse_x);
        MainItemPreview.SetModelOrientation(orientation);
        if (!is_dragging)
        {
            m_MainItemPreviewOrientation = orientation;
        }
    }

    override bool OnMouseWheel(Widget w, int x, int y, int wheel)
    {
        if (w == MainItemPreview)
        {
            m_MainItemPreviewScale = wheel;
            float mw, mh, mx, my;
            MainItemPreview.GetPos(mx, my);
            MainItemPreview.GetSize(mw, mh);
            mw = mw + (m_MainItemPreviewScale / 4);
            mh = mh + (m_MainItemPreviewScale / 4);
            if (mw > 0.5 && mw < 3)
            {
                MainItemPreview.SetSize(mw, mh);
            }
            return true;
        }
        return false;
    }
}
