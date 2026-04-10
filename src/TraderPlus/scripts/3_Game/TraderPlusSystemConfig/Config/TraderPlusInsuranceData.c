class TraderPlusInsuranceData
{
    string classname;
    int price;
    bool isAssured;
    int index;

    void TraderPlusInsuranceData(string cn, int pr, bool assured, int idx)
    {
        classname = cn;
        price = pr;
        isAssured = assured;
        index = idx;
    }
}
