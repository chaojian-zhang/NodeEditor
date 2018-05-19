#define _CRT_SECURE_NO_WARNINGS
#include "ShaderResource.h"
#include <stdio.h>
#include <iostream>
#include <IL\il.h>
#include "MaterialTextures.h"
#include "SystemWrapper.h"
#include "Constants.h"
#include <iostream>

ShaderResource::ShaderResource()
	:shaders{}
{
	// Shader Programs
	FILE* f = fopen(ApplicationPath::PrecompiledProgramFilePath, "rb");	// Open in Read mode will fail if the file doesn't exist, this is what we expect
	for (int i = 0; i < NumberOfPrograms; i++)
	{
		// Generate Program Objects
		shaders[i] = glCreateProgram();

		// Variables
		GLenum formatToken;
		void* binary;
		GLsizei length;
		GLint status;

		if (f != NULL)
		{
			// If precompiled shaders can be found, use them

			// Read parameters
			fread(&length, sizeof(GLsizei), 1, f);
			fread(&formatToken, sizeof(GLenum), 1, f);
			binary = malloc(length);
			fread(binary, length, 1, f);

			// Feed to program
			glProgramBinary(shaders[i], formatToken, binary, length);
			glGetProgramiv(shaders[i], GL_LINK_STATUS, &status);

			free(binary);

			// __Debug__: request more robust solution
			if (status == GL_FALSE)
			{
				std::cout << "[Error]Serious Problem Encountered: Shader Programs cannot be loaded. This will cause rendering errors. The application chooses to ignore this problem though. " << 
					"To solve the problem, close the application, and delete Shaders.program file. Then reload." << std::endl;
			}
		}
		else
		{
			// Otherwise compile new ones and save them
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			GLint vSourceLength = GetShaderSource(i, 0);
			GLint fSourceLength = GetShaderSource(i + NumberOfPrograms, 1);
			glShaderSource(vertexShader, 1, &tempBuffers[0], &vSourceLength);
			glShaderSource(fragmentShader, 1, &tempBuffers[1], &fSourceLength);
			glCompileShader(vertexShader);
			glCompileShader(fragmentShader);
			// Debug_Temp
			char buffer[1024];
			GLsizei length;
			glGetShaderInfoLog(vertexShader, 1024, &length, buffer);
			// Shader program
			glAttachShader(shaders[i], vertexShader);
			glAttachShader(shaders[i], fragmentShader);
			glProgramParameteri(shaders[i], GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
			glLinkProgram(shaders[i]);
			// Delete shaders since we no longer need them
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			delete tempBuffers[0];
			delete tempBuffers[1];
			// Debug_Temp
			GLint status;
			glGetProgramiv(shaders[i], GL_LINK_STATUS, &status);
			if (status == GL_FALSE)
			{
				char buffer[1024];
				int length;
				glGetProgramInfoLog(shaders[i], 1024, &length, buffer);
				std::cout << "[Error]Shader Compilation Error: " << std::endl << buffer << std::endl;	// For some reason debug context isn't working well in this case
				__debugbreak();
			}
		}
	}

	if (f != NULL) bCached = true;

	// Icon Map
	char pathBuffer[MAX_PATH];
	iconMap = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath, pathBuffer));
}

ShaderResource::~ShaderResource()
{
	if (bCached)
	{
		// Close Shader Programs
		for (int i = 0; i < NumberOfPrograms; i++)
		{
			// Destroy programs
			glDeleteProgram(shaders[i]);
		}
	}
	else
	{
		// Save Shader Program until we are closing the application, so pre-compiled shaders are more variant
		char pathBuffer[MAX_PATH];
		FILE* f = fopen(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::PrecompiledProgramFilePath, pathBuffer), "wb");

		if (f == NULL)
		{
			std::cout << "[Error] Cannot Open File to Save Binary Programs." << std::endl;
		}

		// Save Shader Programs
		for (int i = 0; i < NumberOfPrograms; i++)	// We used to have a problem here, that might due to non-existence of our PhongProgram at that time
		{
			// Get Binary Size
			GLsizei size;
			GLenum formatToken;
			char* binaryBuffer;
			glGetProgramiv(shaders[i], GL_PROGRAM_BINARY_LENGTH, &size);
			binaryBuffer = (char*)malloc(size);
			//glGetProgramBinary(shaders[i], size, NULL, &formatToken, &binaryBuffer);	// Classical Bug
			glGetProgramBinary(shaders[i], size, NULL, &formatToken, binaryBuffer);
			// https://support.microsoft.com/en-us/kb/2718841

			fwrite(&size, sizeof(GLsizei), 1, f);
			fwrite(&formatToken, sizeof(GLenum), 1, f);
			fwrite(binaryBuffer, size, 1, f);

			free(binaryBuffer);

			// Destroy programs
			glDeleteProgram(shaders[i]);
		}

		fclose(f);
	}

	// Icon Map is managed by MaterialTexture
}

GLuint ShaderResource::GetShader(Programs program)
{
	return shaders[program];
}

GLuint ShaderResource::GetAppIconMap()
{
	return iconMap;
}

unsigned int ShaderResource::GetShaderSource(unsigned int index, unsigned int tempSlot)
{
	char pathBuffer[MAX_PATH];
	FILE* file = fopen(SystemFunctions::GetExecutableDirectoryPath(ShaderPath::Programs[index], pathBuffer), "rb");

	// Get file size
	fseek(file, 0L, SEEK_END);
	unsigned int fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	// Create buffer
	tempBuffers[tempSlot] = (char*)malloc(fileSize);

	// Read content
	fread(tempBuffers[tempSlot], fileSize, 1, file);

	return fileSize;
}