class SafeZoneNotificationUI extends UIScriptedMenu
{
  private Widget m_RootWidget;
  private RichTextWidget  m_NotifText;

  private int m_countdown;
  private int m_startcountdown;
  private string m_msg;

  void SafeZoneNotificationUI()
  {
		m_RootWidget   = GetGame().GetWorkspace().CreateWidgets("SafeZone/gui/SafeZoneNotification.layout");
	  m_NotifText    = RichTextWidget.Cast( m_RootWidget.FindAnyWidget("NotifText") );
    m_RootWidget.Show(true);
  }

  void Delete()
  {
    m_RootWidget.Show(false);
    delete this;
  }

  void ~SafeZoneNotificationUI()
  {
    GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(CountdownUpdate);
    GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(Close);
  }

  void StartCountdown(int i, string msg)
  {
    m_msg = msg;
    m_countdown = i;
    m_startcountdown = i;
    m_NotifText.SetText(msg + m_countdown.ToString());
    GetGame().GetCallQueue (CALL_CATEGORY_GUI).CallLater(CountdownUpdate, 1000, true);
  }

  void CountdownUpdate()
  {
     m_countdown--;
     m_NotifText.SetText(m_msg + m_countdown.ToString());
    if(m_countdown == 0)
    {
      m_countdown = m_startcountdown;
      GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(CountdownUpdate);
      Delete();
    }
  }
};
