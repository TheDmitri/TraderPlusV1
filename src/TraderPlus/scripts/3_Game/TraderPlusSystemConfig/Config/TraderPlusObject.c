class TraderPlusObject
{
  string ObjectName;
	vector Position, Orientation;

	void TraderPlusObject(string name, vector pos, vector ori)
	{
		ObjectName  =name;
		Position    =pos;
		Orientation =ori;
	}
}
