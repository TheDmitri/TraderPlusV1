modded class PlayerBase extends ManBase
{
	ref TraderPlusBankingMenu m_TraderPlusBankingMenu;
	ref TraderPlusBankingData m_BankAccount = NULL;

	override void Init()
	{
		super.Init();
	}

	TraderPlusBankingData GetBankAccount()
	{
		if(m_BankAccount)
			delete m_BankAccount;

		m_BankAccount = TraderPlusBankingData.GetAccount(this);
		m_BankAccount.UpdateAccount(this);

		return m_BankAccount;
	}



	override void SetActions()
  {
		super.SetActions();
		AddAction(ActionTraderPlusBankingMenu);
  }
};
