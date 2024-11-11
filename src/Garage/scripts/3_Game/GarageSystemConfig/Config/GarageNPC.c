class GarageNPC
{
	string ClassName;
	vector Position;
	vector Orientation;
	vector ParkingPosition;
	vector ParkingOrientation;
	ref    TStringArray Clothes;

	void GarageNPC(string classname, vector pos, vector ori, vector parkpos, vector parkori)
	{
		ClassName = classname;
		Position = pos;
		Orientation = ori;
		Clothes = new TStringArray;
		ParkingPosition = parkpos;
		ParkingOrientation = parkori;
	}

	void AddClothe(string clothe)
	{
		Clothes.Insert(clothe);
	}
}
