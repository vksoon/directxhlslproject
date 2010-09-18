#include "../Headers/FontManager.h"

void FontManager::FMCreateFont(std::string name, int size, LPCSTR font)
{
	std::string id = name;

	ID3DXFont* newFont;

	D3DXCreateFont( D3DObj::Instance()->GetDeviceClass(), size, 0, 0, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,                DEFAULT_PITCH | FF_DONTCARE, TEXT(font), &newFont );

	m_fonts[id] = newFont;
}

void FontManager::FMDrawText(std::string fontid, int x, int y, int width, int height, LPCTSTR text, int r, int g, int b)
{
	RECT pRect;
	pRect.left = x;
	pRect.right = x + width;
	pRect.top = y;
	pRect.bottom = y + height;

	D3DCOLOR fontColour = D3DCOLOR_ARGB(255,r,g,b);  

	FMGetFont(fontid)->DrawText( NULL, text, -1, &pRect, 0, fontColour );
}

void FontManager::FMClean()
{
	for(Fonts::iterator it = m_fonts.begin(); it != m_fonts.end(); ++it)
	{
		ID3DXFont* font = it->second;
		font->Release();
	}
}