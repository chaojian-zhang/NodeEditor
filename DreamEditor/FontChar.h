#pragma once
class FontChar
{
public:
	unsigned int id;
	unsigned short x;
	unsigned short y;
	unsigned short width;
	unsigned short height;
	short xoffset;
	short yoffset;
	unsigned short xadvance;
	unsigned char page;
	unsigned char channel;
};