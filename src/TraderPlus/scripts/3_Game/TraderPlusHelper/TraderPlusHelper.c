static string TraderPlusTrimSpaces(string line)
{
	line.Replace("	", "");
	bool HasSpaces = true;
	while(HasSpaces)
	{
		line = line.Trim();
		if (line.Length() > 0)
			HasSpaces = line[0] == " " || line[line.Length() - 1] == " ";
		else
			HasSpaces = false;
	}
	return line;
}