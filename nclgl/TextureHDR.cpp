#include "TextureHDR.h"
#include <stb_image/stb_image.h>

TextureHDR::TextureHDR(const std::string& filePath) : Texture(filePath)
{
	stbi_set_flip_vertically_on_load(true);
	m_Data = (float*)stbi_loadf(filePath.c_str(), &m_Width, &m_Height, &m_Channel, 0);
	
	if (!m_Data)
	{
		m_IsInitialized = false;
		return;
	}

	glDeleteTextures(1, &m_ProgramID);
	glGenTextures(1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_2D, m_ProgramID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_Data);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_Data)
		stbi_image_free(m_Data);

	stbi_set_flip_vertically_on_load(true);

	m_IsInitialized = true;
}