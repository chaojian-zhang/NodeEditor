#include "Attribute.h"
#include "TextSpecParser.h"
#include <iostream>
#include "SystemWrapper.h"
using namespace std;

//// **** Parser Test
//int main()
//{
//	TextSpecParser parser = TextSpecParser(SystemFunctions::GetExecutableDirectoryPath("Test Document\\TestFile.spec"));
//	// TextSpecParser parser = TextSpecParser(SystemFunctions::GetExecutableDirectoryPath("MeshObjectSpecTemplate.spec")); 
//
//	parser.PrintAttributeStructure();
//
//	cout << "Parsing Done." << endl;
//	std::cin.get();
//
//	const float* values = parser.GetFloatArray("Color", 3);
//
//	return 0;
//}


// **** System Wrapper Test
//int main()
//{
//	const char* filename1 = SystemFunctions::GetFilename("H:\\Directory\\MyFile.file");
//	const char* filename2 = SystemFunctions::GetFilename("H:\\Directory\\Direcoty\\");
//	const char* filename3 = SystemFunctions::GetFilename("MyFile.file");
//
//	cout << "filename1: " << filename1 << endl;
//	cout << "filename2: " << filename2 << endl;
//	cout << "filename3: " << filename3 << endl;
//	std::cin.get();
//
//	return 0;
//}

// **** System Wrapper Test: File Directory
//void PrintTest(const char* string)
//{
//	cout << string << " is ";
//	if (SystemFunctions::IsFileInDataDiretory(string))
//	{
//		cout << "a file inside data path." << endl;
//	}
//	else
//	{
//		cout << "not a file inside data path." << endl;
//	}
//}
//int main(int argc, char** argv)
//{
//	/*
//	File
//		H:\\directory\\filename	-- abs path
//		.\\filename	-- relative path
//		filename	-- relative path
//		\\dir\\filename	-- relative path, unexpected
//	Directory
//		H:\\directory\\directory	-- External Input e.g.user input(not used) or Drag - n - Drop
//		H:\\directory\\directory\\	--WinAPI usage(yes, obviously windows itself isn't consitent about how its api works)
//		.\\directory	-- relative path
//		directory	-- relative path
//		.\\directory\\	--Internal
//		\\dir\\dir -- relative path, unexpected
//	*/
//
//	PrintTest("H:\\directory\\filename");
//	PrintTest(".\\filename");
//	PrintTest("filename");
//	PrintTest("\\dir\\filename");
//	PrintTest("H:\\directory\\directory");
//	PrintTest("H:\\directory\\directory\\");
//	PrintTest(".\\directory");
//	PrintTest("directory");
//	PrintTest(".\\directory\\");
//	PrintTest("\\dir\\dir");
//	PrintTest(argv[0]);
//	PrintTest(argv[1]);
//	PrintTest("C:\\Users\\Administrator\\Desktop\\Node Editor\\NodeEditor\\Debug\\DreamEditor\\Data\\CharMap_0.png");
//	PrintTest("C:\\Users\\Administrator\\Desktop\\Node Editor\\NodeEditor\\Debug\\DreamEditor\\Test Document\\Data");
//	PrintTest("C:\\Users\\Administrator\\Desktop\\Node Editor\\NodeEditor\\Debug\\DreamEditor\\Test Document\\Data\\file.txt");
//
//	cin.get();
//
//	return 0;
//}

