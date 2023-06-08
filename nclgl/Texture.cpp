#include "Texture.h"

Texture::Texture(const std::string& filePath) :
	m_ProgramID(0),
	m_FilePath(filePath),
	m_Width(0),
	m_Height(0),
	m_Channel(0),
	m_Data(nullptr)
{
	//m_Data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_Channel, 0);
	m_Data = SOIL_load_image(m_FilePath.c_str(), &m_Width, &m_Height, &m_Channel, 0);

	if (!m_Data)
	{
		m_IsInitialized = false;
		return;
	}

	GLenum format = GL_RGBA;
	if (m_Channel == 1)
		format = GL_RED;
	else if (m_Channel == 3)
		format = GL_RGB;
	else if (m_Channel == 4)
		format = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_Data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "File: " << m_FilePath << ", Channels: " << m_Channel << ", Format: " << std::to_string(format) << ", Program ID: " << m_ProgramID << std::endl;

	m_IsInitialized = true;

	if(m_Data)
		SOIL_free_image_data(m_Data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ProgramID);
}
