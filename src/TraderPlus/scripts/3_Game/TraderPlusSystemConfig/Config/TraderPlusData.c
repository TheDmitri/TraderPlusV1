class TraderPlusData
{
	int 							Id;
  string  					Name;
	string            GivenName;
	string            Role;
	vector						Position;
	vector						Orientation;
	ref array<string> Clothes;

  void TraderPlusData(int id, string name,string givenname,string role,vector pos, vector ori)
  {
    Id 								= id;
    Name 							= name;
		GivenName   			= givenname;
		Role        			= role;
    Position 					= pos;
    Orientation 			= ori;
    Clothes 					= new array<string>;
  }

  void AddClotheToTraderPlus(string clothename)
  {
    Clothes.Insert(clothename);
  }
};
