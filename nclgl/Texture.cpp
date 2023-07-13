#include "Texture.h"
#include <stb_image/stb_image.h>

Texture::Texture(const unsigned int& width, const unsigned int& height) :
	m_ProgramID(0),
	m_FilePath(""),
	m_Width(width),
	m_Height(height),
	m_Channel(0),
	m_InternalFormat(GL_RGBA),
	m_Format(GL_RGBA),
	m_Type(GL_UNSIGNED_BYTE),
	m_Data(nullptr)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_Type, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_IsInitialized = true;
}

Texture::Texture(const unsigned int& width, const unsigned int& height, const int& internalFormat, const int& normalFormat) :
	m_ProgramID(0),
	m_FilePath(""),
	m_Width(width),
	m_Height(height),
	m_Channel(0),
	m_InternalFormat(internalFormat),
	m_Format(normalFormat),
	m_Type(GL_UNSIGNED_BYTE),
	m_Data(nullptr)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_Type, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_IsInitialized = true;
}

Texture::Texture(const unsigned int& width, const unsigned int& height, const int& internalFormat, const int& normalFormat, const int& type) :
	m_ProgramID(0),
	m_FilePath(""),
	m_Width(width),
	m_Height(height),
	m_Channel(0),
	m_InternalFormat(internalFormat),
	m_Format(normalFormat),
	m_Type(type),
	m_Data(nullptr)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_Type, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_IsInitialized = true;
}

Texture::Texture(const std::string& filePath, const unsigned int& width, const unsigned int& height) :
	m_ProgramID(0),
	m_FilePath(filePath),
	m_Width(width),
	m_Height(height),
	m_Channel(0),
	m_InternalFormat(GL_RGBA),
	m_Format(GL_RGBA),
	m_Type(GL_UNSIGNED_BYTE),
	m_Data(nullptr)
{
	Validate();
}

Texture::Texture(const std::string& filePath, bool shouldValidate) :
	m_ProgramID(0),
	m_FilePath(filePath),
	m_Width(0),
	m_Height(0),
	m_Channel(0),
	m_InternalFormat(GL_RGBA),
	m_Format(GL_RGBA),
	m_Type(GL_UNSIGNED_BYTE),
	m_Data(nullptr)
{
	if(shouldValidate)
		Validate();
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ProgramID);
	m_ProgramID = 0;
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Validate()
{
	stbi_set_flip_vertically_on_load(true);
	m_Data = (unsigned char*)stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_Channel, 0);	
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

	m_InternalFormat = m_Format = format;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_Type, m_Data);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "File: " << m_FilePath << ", Channels: " << m_Channel << ", Format: " << std::to_string(format) << ", Program ID: " << m_ProgramID << std::endl;

	if (m_Data)
		stbi_image_free(m_Data);
	
	stbi_set_flip_vertically_on_load(false);

	m_IsInitialized = true;
}
