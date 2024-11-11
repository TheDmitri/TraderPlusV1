#ifndef CARLOCKDISABLE
class CarLockMenu extends UIScriptedMenu
{
  private   ButtonWidget      m_BttnOK;
  private   ButtonWidget      m_BttnCancel;
  private   EditBoxWidget     m_PasswordInput;
  private   TextWidget        m_TextOutPut;
  private   CarScript         m_Car;


  void CarLockMenu(CarScript car)
  {
    m_Car = car;
  }

  void Show(bool rep)
  {
    if(rep)
    {
      GetGame().GetInput().ChangeGameFocus(1);
      GetGame().GetUIManager().ShowUICursor(true);
      GetGame().GetMission().GetHud().Show(false);
      OnShow();
    }
    else
    {
      GetGame().GetInput().ResetGameFocus();
      GetGame().GetUIManager().ShowUICursor(false);
      GetGame().GetMission().GetHud().Show(true);
      OnHide();
    }
    layoutRoot.Show(rep);
  }

  override void OnShow()
	{
		super.OnShow();
		PPEffects.SetBlurMenu(0.5);
		GetGame().GetInput().ChangeGameFocus(1);
		SetFocus( layoutRoot );
    UIHandle();
	}

	override void OnHide()
	{
		super.OnHide();
		PPEffects.SetBlurMenu(0);
		GetGame().GetInput().ResetGameFocus();
		Close();
	}

  override Widget Init()
    {
		    layoutRoot = GetGame().GetWorkspace().CreateWidgets( "CarLock/gui/CarLockUI.layout" );
        m_BttnOK = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnOK" ) );
        m_BttnCancel = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnCancel" ) );
        m_PasswordInput = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "PasswordInput" ) );
        m_TextOutPut  = TextWidget.Cast(layoutRoot.FindAnyWidget( "TextOutPut" ));
        layoutRoot.Show(false);
        return layoutRoot;
    }

  void UIHandle()
  {
    if(m_Car.m_CarLockOwner != -1 && m_Car.m_CarLockPassword==-1)
    {
      m_TextOutPut.SetText("DEFINE PASSWORD (8)");
    }
    else
    {
      m_TextOutPut.SetText("ENTER PASSWORD (8)");
    }
  }

  override bool OnClick (Widget w, int x, int y, int button)
  {
    switch(w)
    {
      case m_BttnOK: PasswordHandler();
      return true;
      break;

      case m_BttnCancel: OnHide();
      return true;
      break;
    }
    return false;
  }

  bool HasCharacterInPassword(string pwd)
  {
    for(int i=0;i<pwd.Length();i++)
    {
      switch(pwd[i])
      {
        case "0":continue;
        break;

        case "1":continue;
        break;

        case "2":continue;
        break;

        case "3":continue;
        break;

        case "4":continue;
        break;

        case "5":continue;
        break;

        case "6":continue;
        break;

        case "7":continue;
        break;

        case "8":continue;
        break;

        case "9":continue;
        break;

        default:return true;
        break;
      }
    }
    return false;
  }

  void PasswordHandler()
  {
    string password = m_PasswordInput.GetText();
    int passwordAttempt = password.ToInt();

    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

    if(password.Length() < 8)
    {
      m_TextOutPut.SetText("Too short!");
      return;
    }

    if(password.Length() > 8)
    {
      m_TextOutPut.SetText("Too long!");
      return;
    }

    if(HasCharacterInPassword(password))
    {
      m_TextOutPut.SetText("Number only !");
      return;
    }

    if(passwordAttempt && m_Car.m_CarLockOwner == -1 || (m_Car.m_CarLockOwner==player.CLSteamlowID && m_Car.m_CarLockPassword == -1))
    {
      //GetTraderPlusLogger().LogInfo("passwordAttempt && m_Car.m_CarLockOwner == -1");
      GetRPCManager().SendRPC("CarLock", "CarLockPasswordRequest",  new Param2<CarScript, int>(m_Car,passwordAttempt), true, NULL);
      CarLockData carLockDataAdmin = CarLockData.Load(m_Car.m_CarLockOwner.ToString());
      if(carLockDataAdmin.CarLockPasswords.Find(passwordAttempt) == -1)
      {
        carLockDataAdmin.CarLockPasswords.Insert(passwordAttempt);
        carLockDataAdmin.Save(m_Car.m_CarLockOwner.ToString());
      }
      OnHide();
      return;
    }
    if(passwordAttempt == m_Car.m_CarLockPassword)
    {
      GetRPCManager().SendRPC("CarLock", "CarLockRequest",  new Param2<CarScript, bool>(m_Car,false), true, NULL);
      CarLockData carLockData = CarLockData.Load(m_Car.m_CarLockOwner.ToString());
      if(carLockData.CarLockPasswords.Find(m_Car.m_CarLockPassword) == -1)
      {
        carLockData.CarLockPasswords.Insert(m_Car.m_CarLockPassword);
        carLockData.Save(m_Car.m_CarLockOwner.ToString());
      }
      OnHide();
      return;
    }
  }
};
#endif
