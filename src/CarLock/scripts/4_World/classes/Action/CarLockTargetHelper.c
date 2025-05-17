class CarLockTargetHelper
{
    static CarScript GetTargetCar(ActionTarget target)
    {
        CarScript car = CarScript.Cast(target.GetObject());

        if (!car){
            car = CarScript.Cast(target.GetParent());
        }

        return car;
    }
}