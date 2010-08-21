#include "../Headers/File.h"

bool File::Open(const std::string& filename)			//File handling class. Used to open the level files
{														//and read in the enemy types
	m_ifstream.open(filename.c_str(), std::ios::in);	// 
	return m_ifstream.good();							//Getting the damn thing to work was almost more
}														// hassle than it was worth

bool File::GetInt(int* pInt)
{
    std::string s;
    if (!GetString(&s))
    {
        return false;
    }
    *pInt = atoi(s.c_str());
	return true;
}

bool File::GetFloat(float* pFloat)
{
	m_ifstream >> (*pFloat);
	return true;
}

bool File::GetString(std::string* pString)
{
	char buf[10000];
    while (true)
    {
        if (EndOfFile())
        {
            return false;
        }

	    m_ifstream.getline(buf, 10000); 
	    *pString = buf;

        if (pString->size() > 0 && (*pString)[0] == '#')
        {
            continue;
        }
        if (!pString->empty())
        {
	        return true;
        }
		
	
    }
}

bool File::EndOfFile() const
{
	return m_ifstream.eof() || !m_ifstream.good();
}
