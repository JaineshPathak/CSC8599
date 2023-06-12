#pragma once
#include <string>
#include <glad/glad.h>
#include <iostream>

class Texture
{
public:
	Texture() = delete;
	Texture(const std::string& filePath);	
	virtual ~Texture();

	unsigned int GetID() const { return m_ProgramID; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }

	bool IsInitialized() const
	{ 
		if (!m_IsInitialized)
			std::cout << "Texture: " << m_FilePath << " is not initialized!" << std::endl;

		return m_IsInitialized; 
	}

protected:
	bool m_IsInitialized;
	unsigned int m_ProgramID;

	std::string m_FilePath;
	int m_Width, m_Height, m_Channel;
	void* m_Data;
};