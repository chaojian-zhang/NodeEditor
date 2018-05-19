#pragma once
#include <vector>	// Used specifically for Convert Strings
#include <Windows.h>
#include "Time.h"

// INput: threadLocalBuffer should be zeroed out
class SystemFunctions
{
public:
	static char* GetWorkingDirectoryPath(const char* relativePath, char* threadLocalBuffer);	// Convention: Relative Path be either ".\\Something\\Somethingmore", or "Something\\SomethingMore"; shouldn't be "\dir\file"
	static char* GetExecutableDirectoryPath(const char* relativePath, char* threadLocalBuffer);	// Not expecting "\dir\file"
	// Caution: Except the above two functions, all path/filename parameters in other function require abs path
	static char* GetDocumentDataPath(const char* fileLocation, char* threadLocalBuffer);	// Provide a stack allocated buffer on working thread to make this function thread safe, otherwise we will use internal buffer; A better way is to force use stack allocated buffer but we don't wish to change too much existing code
	static char* GetExecutableDataPath(const char* fileLocation, char* threadLocalBuffer);
	
	// String Conversion
	static int U16ToU8(unsigned short* U16String, unsigned int numChars, char* U8String = 0, unsigned int size = 0);	// When U8String == NULL or size == 0, we return the size only; not null Terminated so we need to manually + 2
	static int U16ToANSI(unsigned short* U16String, unsigned int numChars, char* ANSI = 0, unsigned int size = 0);	// Assume not null terminated so we need to +1 to the returned size
	static int ANSIToU16(const char* ANSIString, unsigned short* U16String = NULL, unsigned int numChars = 0);	// Expect ANSIString null terminated; Returned String NULL Terminated; Return Num of UShort characters
	/* References
	https://msdn.microsoft.com/en-us/library/windows/desktop/dd318026%28v=vs.85%29.aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/dd319072%28v=vs.85%29.aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/dd374130%28v=vs.85%29.aspx
	http://unicode-table.com/en/0000/
	https://support.microsoft.com/en-us/kb/138813
	*/

	// Usage Note: TO convert from U8 to ANSI: first convert to U16, then convert to ANSI
	static std::wstring ConvertU8toU16(const std::string& utf8);

	// File Name Path handling
	// Return NULL if input is NULL or input is a directory, i.e. of the form "H:\\Someplace\\Directory\\"
	static const char* GetFilename(const char* filepath);	// Pass in a string, find the location of the filename, i.e. not directries, return the pointer to that location
	// Expecting Abs path
	// Return whether the path is in either document or executable data directory; 
	// Return false if just a relative filename, Return false if not a file but a directory
	// Also return whether it is a relative filename
	// Also return fileLocation if it is inside Data Directory
	static bool IsFileInDataDiretory(const char* filepath, char* threadLocalBuffer, bool* bRelativePath = NULL, const char** fileLocation = NULL);
	// Copy the file into document data path; the user is responsible for making sure it is not already there, otherwise we will override existing ones
	// Return false if fail
	static bool CopyFileToDocumentData(const char* filepath, char* threadLocalBuffer);
	// Return File Exist Or Not
	// Input should be abs path
	static bool IsFileExist(const char* filepath);
	// Pass in a string, convert it to proper ANSI texts ready to use as file name in the OS
	static const char* CreateFilename(const unsigned short* U16String, unsigned int numChars, char* threadLocalBuffer);

	// File System Interaction
	static void OpenFileExternal(const char* filepath);	// Open a file/directory using external applications

private:
	// static char* GetGraphFileDirectory();	// Working Directory
	// static char* GetApplicationDirectory();	// Executable Directory
	static char* GetWorkingDirectory(char* threadLocalBuffer);
	static char* GetExecutableDirectory(char* threadLocalBuffer);
	// Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197%28v=vs.85%29.aspx
	// Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/aa364934%28v=vs.85%29.aspx
	// Reference(Not read actually): https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247%28v=vs.85%29.aspx
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364989%28v=vs.85%29.aspx
	// Need to read the above two some day though, so that we are not limited to 260 length file path
	// http://stackoverflow.com/questions/21402742/fopen-with-unicode-filename

private:
	// static char pathBuffer[MAX_PATH];	// Used for keeping internally the path and file results
};

/*Regarding wide characters in windows: http://stackoverflow.com/questions/1810343/is-a-wide-character-string-literal-starting-with-l-like-lhello-world-guarantee
So  we have the following assumptions:
	The main document file, when we do not use L"" to start an literal, since we seletect Use Multi-Byte Character Set in CharSet option of the project, it should be in ANSI;
	While for explicit L"" literals, it would be in Unicode 16. More specific informaion pending refering to MSDN pages.
*/

/* Filenames and paths in our application have below forms:
	File
		H:\\directory\\filename	-- abs path
		.\\filename	-- relative path
		filename	-- relative path
		\\dir\\filename	-- relative path, unexpected
	Directory
		H:\\directory\\directory	-- External Input e.g. user input(not used) or Drag-n-Drop
		H:\\directory\\directory\\	-- WinAPI usage (yes, obviously windows itself isn't consitent about how its api works)
		.\\directory	-- relative path
		directory	-- relative path
		.\\directory\\	-- Internal
		\\dir\\dir -- relative path, unexpected
*/