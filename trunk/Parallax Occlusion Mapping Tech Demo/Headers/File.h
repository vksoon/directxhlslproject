#ifndef FILE_H
#define FILE_H

#include <fstream>

class File
{
public:
	bool Open(const std::string& filename);

	bool GetInt(int* pInt);

	bool GetFloat(float* pFloat);

	bool GetString(std::string* pString);

	bool EndOfFile() const;

private:
	std::ifstream m_ifstream;
};

#endif