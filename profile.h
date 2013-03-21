#include "recognition.h"


#ifndef PROFILE_INCLUDED
#define PROFILE_INCLUDED

class Profile
{
public:
	Profile();
	~Profile();

	void Save();
	void Load();

private:
	void SaveData(TWL::XMLElement &xmlElement);
	void SaveSettings(TWL::XMLElement &xmlElement);

	void LoadData(TWL::XMLElement &xmlElement);
	void LoadSettings(TWL::XMLElement &xmlElement);

	TWL::String sDocument;

};



#endif