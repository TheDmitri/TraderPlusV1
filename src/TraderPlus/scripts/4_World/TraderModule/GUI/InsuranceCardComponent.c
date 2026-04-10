class InsuranceCardComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("insurance-card-component", InsuranceCardComponent);

    static ref ScriptInvoker Event_OnInsuranceSelected = new ScriptInvoker();

    // Input — set by ForEach binding via _input prefix
    ref TraderPlusInsuranceData _inputInsurance;

    // Bound properties
    string displayName;
    int priceValue;
    string statusText;
    EntityAI preview;

    // Widgets
    Widget InsuranceStatut;

    // Internal
    private EntityAI m_localEntity;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "insurance-card-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/InsuranceCardComponent.layout";
    }

    override void OnInit()
    {
        if (!_inputInsurance)
        {
            return;
        }

        displayName = TraderPlusHelper.GetDisplayName(_inputInsurance.classname);
        priceValue = _inputInsurance.price;

        if (_inputInsurance.isAssured)
        {
            statusText = "#tpm_vehicle_assured";
            if (InsuranceStatut)
            {
                InsuranceStatut.SetColor(ARGB(255, 18, 143, 49));
            }
        }
        else
        {
            statusText = "#tpm_claim_insurance_money";
            if (InsuranceStatut)
            {
                InsuranceStatut.SetColor(ARGB(255, 209, 137, 60));
            }
        }

        // Create preview
        m_localEntity = EntityAI.Cast(GetGame().CreateObjectEx(_inputInsurance.classname, vector.Zero, ECE_LOCAL | ECE_NOLIFETIME));
        if (m_localEntity)
        {
            preview = m_localEntity;
        }

        MarkForCheck();
    }

    override void OnDestroy()
    {
        if (m_localEntity)
        {
            m_localEntity.Delete();
            m_localEntity = null;
        }
    }

    override void OnChanges()
    {
        if (m_localEntity)
        {
            m_localEntity.Delete();
            m_localEntity = null;
        }
        OnInit();
    }

    void OnInsuranceClicked(ClickEventArgs args)
    {
        if (_inputInsurance)
        {
            Event_OnInsuranceSelected.Invoke(_inputInsurance.index);
        }
    }
}
