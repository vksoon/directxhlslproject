#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "D3DObject.h"
#include "Singleton.h"
#include <map>

class FontManager
{
public:

	~FontManager() {}

	void FMCreateFont(std::string name, int size, LPCSTR font);
	void FMDrawText(std::string fontid, int x, int y, int width, int height, LPCTSTR text, int r, int g, int b);

	void FMClean();

	ID3DXFont * FMGetFont(std::string id) { return m_fonts[id]; }

protected:

	FontManager() {}
	friend class Singleton<FontManager>;

private:

	typedef std::map<std::string, ID3DXFont*> Fonts;
	Fonts m_fonts;
};

typedef Singleton<FontManager> FontManagerObj;

#endif