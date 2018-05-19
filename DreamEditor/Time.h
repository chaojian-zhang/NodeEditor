#pragma once
// Notice: when later modify this class, if needed, notice do not casually change class size and organization since loadgin and saving depends on it
class Time
{
public:
	unsigned short year;
	unsigned char day;
	unsigned char month;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;

public:
	unsigned short* GetString();	// Used for label elements; Caller is responsible for releasing memory
	unsigned int GetStringLength();	// In case the user is interested in serialize our output and would like to know how long a string we are generating; Doesn't include NULL terminating character

private:
	static unsigned short timeString[20];	// Shared Internal Time String
	// Format: "2015-09-07 05:03:29", total 19 chars + 1 NUll char
};