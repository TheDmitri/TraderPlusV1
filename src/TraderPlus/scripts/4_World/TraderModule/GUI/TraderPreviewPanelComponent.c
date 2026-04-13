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

    // Player preview (created dynamically)
    private PlayerPreviewWidget m_PlayerPreview;
    private int m_CharacterRotationX;
    private int m_CharacterRotationY;

    // Internal state
    private int m_PreviewID;
    private EntityAI m_previewItem;
    private PlayerBase m_previewPlayer;
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

    int GetPreviewID()
    {
        return m_PreviewID;
    }

    void SwitchPreview()
    {
        if (m_PreviewID == TRADERPLUS_MAINITEMPREVIEW)
        {
            // Switch to player preview
            m_PreviewID = TRADERPLUS_PLAYERPREVIEW;

            // Hide item preview widget
            if (PanelWidgetPreview)
            {
                PanelWidgetPreview.Show(false);
            }

            // Init player preview
            InitPlayerPreview();
            UpdatePlayerClothes();

            if (ItemDescription)
            {
                ItemDescription.Show(false);
            }
            if (GridInsurance)
            {
                GridInsurance.Show(false);
            }
            if (BttnReset)
            {
                BttnReset.Show(true);
            }
            if (BttnRemove)
            {
                BttnRemove.Show(true);
            }
            if (ImagePlayerPreview)
            {
                ImagePlayerPreview.SetColor(ARGB(255, 201, 48, 44));
            }
        }
        else if (m_PreviewID == TRADERPLUS_PLAYERPREVIEW)
        {
            // Switch to item preview
            m_PreviewID = TRADERPLUS_MAINITEMPREVIEW;

            // Remove player preview
            RemovePlayerPreview();

            // Show item preview widget
            if (PanelWidgetPreview)
            {
                PanelWidgetPreview.Show(true);
            }
            if (ItemDescription)
            {
                ItemDescription.Show(true);
            }
            if (ImagePlayerPreview)
            {
                ImagePlayerPreview.SetColor(ARGB(255, 255, 255, 255));
            }
            if (BttnReset)
            {
                BttnReset.Show(false);
            }
            if (BttnRemove)
            {
                BttnRemove.Show(false);
            }

            // Restore item preview if product selected
            if (m_CurrentProduct && m_CurrentProduct.ClassName != "")
            {
                UpdateItemPreview(m_CurrentProduct.ClassName);
            }
        }
    }

    void CreatePlayerForPreview()
    {
        if (m_previewPlayer)
        {
            GetGame().ObjectDelete(m_previewPlayer);
        }

        vector dir = GetGame().GetCurrentCameraDirection();
        dir.Normalize();
        vector pos = GetGame().GetCurrentCameraPosition() - dir * 0.5;
        pos[1] = GetGame().GetPlayer().GetPosition()[1];

        m_previewPlayer = PlayerBase.Cast(GetGame().CreateObjectEx(GetGame().GetPlayer().GetType(), pos, ECE_LOCAL | ECE_NOLIFETIME));
        if (m_previewPlayer)
        {
            m_previewPlayer.m_IsStashDisable = true;
        }
    }

    void InsertPlayerToPreview()
    {
        if (m_PlayerPreview && m_previewPlayer)
        {
            m_PlayerPreview.SetPlayer(m_previewPlayer);
            m_PlayerPreview.UpdateItemInHands(m_previewPlayer.GetHumanInventory().GetEntityInHands());
            m_PlayerPreview.SetModelOrientation(m_CharacterOrientation);
            m_PlayerPreview.SetModelPosition(vector.Zero);
            m_PlayerPreview.SetSize(1, 1);
        }
    }

    void InitPlayerPreview()
    {
        if (!m_PlayerPreview && PanelWidgetPreview)
        {
            Widget previewFrame = GetGame().GetWorkspace().CreateWidgets("TraderPlus/gui/PlayerPreview.layout", PanelWidgetPreview);
            if (previewFrame)
            {
                m_PlayerPreview = PlayerPreviewWidget.Cast(previewFrame);
                if (m_PlayerPreview)
                {
                    m_PlayerPreview.Show(true);
                }
            }
        }

        CreatePlayerForPreview();

        if (m_previewPlayer)
        {
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

        InsertPlayerToPreview();
    }

    void RemovePlayerPreview()
    {
        if (m_PlayerPreview)
        {
            Widget playerFrame = m_PlayerPreview;
            if (playerFrame && PanelWidgetPreview)
            {
                PanelWidgetPreview.RemoveChild(playerFrame);
            }
            m_PlayerPreview = null;
        }
        if (m_previewPlayer)
        {
            GetGame().ObjectDelete(m_previewPlayer);
            m_previewPlayer = null;
        }
    }

    void UpdatePlayerClothes()
    {
        if (!m_PlayerPreview)
        {
            InitPlayerPreview();
        }

        if (!m_CurrentProduct || m_CurrentProduct.ClassName == "")
        {
            return;
        }

        int id = TraderPlusHelper.GetSlotForPlayerPreview(m_CurrentProduct.ClassName);

        if (id != -1 && m_previewPlayer)
        {
            EntityAI existingEnt = m_previewPlayer.FindAttachmentBySlotName(TraderPlusHelper.playerAttachments[id]);
            if (existingEnt)
            {
                GetGame().ObjectDelete(existingEnt);
            }
            int slotId = InventorySlots.GetSlotIdFromString(TraderPlusHelper.playerAttachments[id]);
            m_previewPlayer.GetInventory().CreateAttachmentEx(m_CurrentProduct.ClassName, slotId);
        }
        else if (m_previewPlayer)
        {
            EntityAI currentInHand = m_previewPlayer.GetHumanInventory().GetEntityInHands();
            if (currentInHand)
            {
                GetGame().ObjectDelete(currentInHand);
            }
            m_previewPlayer.GetHumanInventory().CreateInHands(m_CurrentProduct.ClassName);
        }

        InsertPlayerToPreview();
    }

    void RemoveAllPlayerClothes()
    {
        if (!m_previewPlayer)
        {
            InitPlayerPreview();
        }
        else
        {
            CreatePlayerForPreview();
            InsertPlayerToPreview();
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

    // --- Quantity multiplier (OnChange for combo box) ---

    override bool OnChange(Widget w, int x, int y, bool finished)
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

        return false;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
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

        if (w == BttnReset)
        {
            InitPlayerPreview();
            return true;
        }

        if (w == BttnRemove)
        {
            RemoveAllPlayerClothes();
            return true;
        }

        return false;
    }

    // --- Player preview rotation via mouse drag ---

    override bool OnMouseButtonDown(Widget w, int x, int y, int button)
    {
        if (w == m_PlayerPreview)
        {
            GetGame().GetDragQueue().Call(this, "UpdatePlayerRotation");
            g_Game.GetMousePos(m_CharacterRotationX, m_CharacterRotationY);
            return true;
        }
        return false;
    }

    void UpdatePlayerRotation(int mouse_x, int mouse_y, bool is_dragging)
    {
        vector orientation = m_CharacterOrientation;
        orientation[1] = orientation[1] - (m_CharacterRotationX - mouse_x);

        if (m_PlayerPreview)
        {
            m_PlayerPreview.SetModelOrientation(orientation);
        }

        if (!is_dragging)
        {
            m_CharacterOrientation = orientation;
        }
    }
}
