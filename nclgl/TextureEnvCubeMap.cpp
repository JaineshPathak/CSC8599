#include "TextureEnvCubeMap.h"

TextureEnvCubeMap::TextureEnvCubeMap(const unsigned int width, const unsigned int height) : Texture(width, height)
{
	Validate();
}

void TextureEnvCubeMap::Validate()
{
	if (m_ProgramID > 0)
		glDeleteTextures(1, &m_ProgramID);

	glGenTextures(1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ProgramID);

	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_IsInitialized = true;
}