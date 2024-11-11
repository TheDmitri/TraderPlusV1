modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(ActionTraderPlusMenu);
        actions.Insert(ActionPackVehicle);
        actions.Insert(ActionDeployVehicle);
        actions.Insert(ActionConvertToNewReceipt);
    }
}
