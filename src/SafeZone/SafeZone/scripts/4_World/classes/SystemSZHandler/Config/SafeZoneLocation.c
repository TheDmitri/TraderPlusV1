class SafeZoneLocation
{
  string SafeZoneStatut;
  float X, Y, Radius;
  int Countdown;

    void SafeZoneLocation( string safezonestatut, float x, float y, float radius, int countdown)
	  {
      SafeZoneStatut = safezonestatut;
      X = x;
      Y = y;
      Radius = radius;
      Countdown = countdown;
    }
}
