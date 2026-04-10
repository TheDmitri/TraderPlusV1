class TraderCurrencyPipe extends DZPipe
{
    override string Transform(string value, array<string> args)
    {
        int intValue = value.ToInt();
        return TraderPlusHelper.IntToCurrencyString(intValue, ",");
    }
}
