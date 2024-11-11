/*
Don't judge me on this but I spent far too much time trying to copy a map
without loosing data from the original one. This method works, so I'm using it !
*/
class TraderPlusCopyMap<Class T>
{
	static void CopyMap(T dataIn, out T dataOut)
	{
		string data = TraderPlusJsonLoader<T>.ObjectToString(dataIn);
		TraderPlusJsonLoader<T>.StringToObject(data, dataOut);
	}
}
