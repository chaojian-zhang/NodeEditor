#define _CRT_SECURE_NO_WARNINGS
#include "SystemWrapper.h"
#include <Shlwapi.h>	// PathFileExist(); ShellExecute()

// Static Variable Definition
// char SystemFunctions::pathBuffer[MAX_PATH] = {};

char* SystemFunctions::GetWorkingDirectoryPath(const char* relativePath, char* threadLocalBuffer)
{
	int length = GetCurrentDirectoryA(MAX_PATH, threadLocalBuffer);
	strcat(threadLocalBuffer, "\\");
	return strcat(threadLocalBuffer, relativePath);
}

char* SystemFunctions::GetExecutableDirectoryPath(const char* relativePath, char* threadLocalBuffer)
{
	GetExecutableDirectory(threadLocalBuffer);
	strcat(threadLocalBuffer, "\\");
	return strcat(threadLocalBuffer, relativePath);
}

char* SystemFunctions::GetDocumentDataPath(const char* fileLocation, char* threadLocalBuffer)
{
	int length = GetCurrentDirectoryA(MAX_PATH, threadLocalBuffer);
	strcat(threadLocalBuffer, "\\Data\\");
	return strcat(threadLocalBuffer, fileLocation);
}

char* SystemFunctions::GetExecutableDataPath(const char* fileLocation, char* threadLocalBuffer)
{
	GetExecutableDirectory(threadLocalBuffer);
	strcat(threadLocalBuffer, "\\Data\\");
	return strcat(threadLocalBuffer, fileLocation);
}

int SystemFunctions::U16ToU8(unsigned short* U16String, unsigned int numChars, char* U8String, unsigned int size)
{
	if (U8String == NULL || size == 0)
	{
		return WideCharToMultiByte(CP_UTF8, NULL, (wchar_t*)U16String, numChars, U8String, 0, NULL, NULL);
	}
	else
	{
		return WideCharToMultiByte(CP_UTF8, NULL, (wchar_t*)U16String, numChars, U8String, size, NULL, NULL);
	}
}

int SystemFunctions::U16ToANSI(unsigned short* U16String, unsigned int numChars, char* ANSIString, unsigned int size)
{
	if (ANSIString == NULL || size == 0)
	{
		return WideCharToMultiByte(CP_ACP, NULL, (wchar_t*)U16String, numChars, ANSIString, 0, NULL, NULL);
	}
	else
	{
		return WideCharToMultiByte(CP_ACP, NULL, (wchar_t*)U16String, numChars, ANSIString, size, NULL, NULL);
	}

	// We leave caller to decide whether or not he should +1 to the size
}

int SystemFunctions::ANSIToU16(const char* ANSIString, unsigned short* U16String, unsigned int numChars)
{
	if (U16String == NULL || numChars == 0)
	{
		return MultiByteToWideChar(CP_ACP, NULL, ANSIString, -1, (wchar_t*)U16String, 0);
	}
	else
	{
		return MultiByteToWideChar(CP_ACP, NULL, ANSIString, -1, (wchar_t*)U16String, numChars);

	}
}

std::wstring SystemFunctions::ConvertU8toU16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		bool error = false;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni = ch;
			todo = 0;
		}
		else if (ch <= 0xBF)
		{
			throw std::logic_error("not a UTF-8 string");
		}
		else if (ch <= 0xDF)
		{
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
			todo = 3;
		}
		else
		{
			throw std::logic_error("not a UTF-8 string");
		}
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size())
				throw std::logic_error("not a UTF-8 string");
			unsigned char ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF)
				throw std::logic_error("not a UTF-8 string");
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF)
			throw std::logic_error("not a UTF-8 string");
		if (uni > 0x10FFFF)
			throw std::logic_error("not a UTF-8 string");
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (size_t i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF)
		{
			utf16 += (wchar_t)uni;
		}
		else
		{
			uni -= 0x10000;
			utf16 += (wchar_t)((uni >> 10) + 0xD800);
			utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
		}
	}

	return utf16;
}

const char* SystemFunctions::GetFilename(const char* filepath)
{
	const char* location = strrchr(filepath, '\\');;
	if (location == NULL)	// Indicates this is just a relative filename
	{
		return filepath;
	}
	else
	{
		return *(location + 1) == 0 ? NULL : location + 1;	// If we are a directory, return NULL, otherwise return next character position
	}
}

bool SystemFunctions::IsFileInDataDiretory(const char* filepath, char* threadLocalBuffer, bool* bRelativePath, const char** fileLocation)
{
	// Set Default Parameter Return value
	if (bRelativePath) *bRelativePath = false;
	if (fileLocation) *fileLocation = NULL;

	// Checking order matters here

	// Check whether input is not a file but a directory; Notice we cannot do further investigation by actually inspect the filepath
	// E.g. H:\\directory\\directory will bypass the test
	if (filepath[strlen(filepath) - 1] == '\\')
	{
		return false;
	}

	// Check whether input is relative
	if (filepath[0] == '.' || filepath[0] == '\\' || strstr(filepath, "\\") == NULL)
	{
		if (bRelativePath) *bRelativePath = true;
		return false;	// We consider relative path as being inside the data directory
	}

	// Check whether inside working directory
	GetWorkingDirectoryPath("Data\\", threadLocalBuffer);
	if (strstr(filepath, threadLocalBuffer))
	{
		*fileLocation = filepath + strlen(threadLocalBuffer);	// Extract fileLocation part
		return true;
	}

	// CHeck wehther inside document directory
	GetExecutableDirectoryPath("Data\\", threadLocalBuffer);
	if (strstr(filepath, threadLocalBuffer))
	{
		*fileLocation = filepath + strlen(threadLocalBuffer); 	// Extract fileLocation part
		return true;
	}

	return false;
}

bool SystemFunctions::CopyFileToDocumentData(const char* filepath, char* threadLocalBuffer)	// Expect abs
{
	// Find filename
	const char* location = strrchr(filepath, '\\');
	const char* filename = location + 1;

	// Generate a destination path
	GetWorkingDirectoryPath(filename, threadLocalBuffer);

	if (CopyFileA(filepath, threadLocalBuffer, false) == 0)	// OVerride exissting file
	{
		// If failed, output a reason, but since FormatMessage() is too complicated
		// We do nothing
		return false;
	}
	return true;
}

const char* SystemFunctions::CreateFilename(const unsigned short* U16String, unsigned int numChars, char* threadLocalBuffer)
{
	// Strip out useless things and clamp to maximum length
	unsigned short* temp = new unsigned short[numChars];
	unsigned int actualNumChars = 0;
	for (unsigned int i = 0; i < numChars && i < MAX_PATH / 2; i++)	// Of course U16 is at most case twice the size as ANSI
	{
		// Exception handling
		if (U16String[i] == 0)
		{
			__debugbreak();
			break;	// Normally shouldn't happen
		}

		if (U16String[i] != L'/' && 
			U16String[i] != L'\\' && 
			U16String[i] != L':' && 
			U16String[i] != L'*' && 
			U16String[i] != L'?' && 
			U16String[i] != L'<' && 
			U16String[i] != L'>' &&
			U16String[i] != L'\"' &&
			U16String[i] != L'|')	// Windows(in U16): \/:*?"<>|
		{
			temp[i] = U16String[i];
		}
		actualNumChars++;
	}
	// Convert to ANSI
	int bytes = U16ToANSI(temp, actualNumChars) + 1;
	if (bytes > MAX_PATH)	// Unexpected Long name
	{
		__debugbreak();
		return NULL;
	}	
	U16ToANSI(temp, actualNumChars, threadLocalBuffer, bytes);
	if (bytes == 1)	// Meaning we have nothing in the name, which isn't good, so give it a default value
	{
		static unsigned int defaultIndex = 0;	// Help eliminate problem with no names
		char defaultFileName[9+4+1] = "Untitled";	// 9 for "Untitled ", 4 for "0001" etc, 1 for NULL
		sprintf(defaultFileName, "Untitled %0.4u", defaultIndex);	// Automatically append a NULL
		defaultIndex++;

		strcpy(threadLocalBuffer, defaultFileName);
		bytes = strlen(defaultFileName) + 1;
	}
	threadLocalBuffer[bytes - 1] = 0;	// Make it null temrinated

	// Return the converted string
	return threadLocalBuffer;
}

void SystemFunctions::OpenFileExternal(const char* filepath)
{
	ShellExecute(NULL, NULL, filepath, NULL, NULL, SW_SHOWNORMAL);
}

char* SystemFunctions::GetWorkingDirectory(char* threadLocalBuffer)
{
	int length = GetCurrentDirectoryA(MAX_PATH, threadLocalBuffer);
	return threadLocalBuffer;
}

char* SystemFunctions::GetExecutableDirectory(char* threadLocalBuffer)
{
	int length = GetModuleFileNameA(NULL, threadLocalBuffer, MAX_PATH);
	// pathBuffer[length] = NULL;// Already done by the above functions

	// Find The Directory, or(equivalently) Strip out the .exe executable name
	unsigned int location = strrchr(threadLocalBuffer, '\\') - threadLocalBuffer;
	threadLocalBuffer[location] = NULL;

	return threadLocalBuffer;
}

bool SystemFunctions::IsFileExist(const char* filepath)
{
	return PathFileExistsA(filepath);
}