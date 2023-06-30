#pragma once
#include <iostream>
#include <string>
#include <glad/glad.h>

class Texture
{
public:
	Texture(const unsigned int width, const unsigned int height);
	Texture(const unsigned int width, const unsigned int height, const int internalFormat, const int normalFormat);
	Texture(const std::string& filePath, const unsigned int width, const unsigned int height);
	Texture(const std::string& filePath, bool shouldValidate = true);	
	virtual ~Texture();

	unsigned int GetID() const { return m_ProgramID; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }

	bool IsInitialized() const
	{ 
		if (!m_IsInitialized)
			std::cout << "Texture: " << m_FilePath << " is not initialized! Program ID: " << m_ProgramID << std::endl;

		return m_IsInitialized; 
	}

protected:
	virtual void Validate();

	bool m_IsInitialized;
	unsigned int m_ProgramID;

	std::string m_FilePath;
	int m_Width, m_Height, m_Channel;
	int m_InternalFormat;
	int m_Format;
	void* m_Data;
};