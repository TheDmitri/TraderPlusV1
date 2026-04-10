class TransactionThrottle
{
    private bool m_canRequest;
    private float m_tick;
    private float m_interval;

    void TransactionThrottle(float interval)
    {
        m_canRequest = true;
        m_tick = 0.0;
        m_interval = interval;
    }

    bool CanRequest()
    {
        return m_canRequest;
    }

    void OnRequestSent()
    {
        m_canRequest = false;
        m_tick = m_interval;
    }

    void Update(float timeslice)
    {
        if (!m_canRequest)
        {
            m_tick -= timeslice;
            if (m_tick <= 0)
            {
                m_tick = 0;
                m_canRequest = true;
            }
        }
    }

    void Reset()
    {
        m_canRequest = true;
        m_tick = 0;
    }
}
