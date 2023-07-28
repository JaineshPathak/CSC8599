#include "TextureCubeMap.h"
#include <stb_image/stb_image.h>

TextureCubeMap::TextureCubeMap(const unsigned int width, const unsigned int height, bool generateMipMaps) :
	m_ShouldGenerateMipMaps(generateMipMaps), Texture(width, height)
{
	Validate();
}

TextureCubeMap::TextureCubeMap(const std::string& X_POS, const std::string& X_NEG, const std::string& Y_POS, const std::string& Y_NEG, const std::string& Z_POS, const std::string& Z_NEG, bool generateMipmaps) :
	m_ShouldGenerateMipMaps(generateMipmaps), Texture(0, 0)
{
	SetTextureFaces(X_POS, X_NEG, Y_POS, Y_NEG, Z_POS, Z_NEG);
}

TextureCubeMap::TextureCubeMap(const std::string texture_faces[], bool generateMipmaps) : 
	m_ShouldGenerateMipMaps(generateMipmaps), Texture(0, 0)
{
	SetTextureFaces(texture_faces);
}

TextureCubeMap::TextureCubeMap(const std::vector<std::string> texture_faces, bool generateMipmaps) : 
	m_ShouldGenerateMipMaps(generateMipmaps), Texture(0, 0)
{
	SetTextureFaces(texture_faces);
}

void TextureCubeMap::SetTextureFaces(const std::string& X_POS, const std::string& X_NEG, const std::string& Y_POS, const std::string& Y_NEG, const std::string& Z_POS, const std::string& Z_NEG)
{
	m_Texture_Faces_Files[0] = X_POS;
	m_Texture_Faces_Files[1] = X_NEG;
	m_Texture_Faces_Files[2] = Y_POS;
	m_Texture_Faces_Files[3] = Y_NEG;
	m_Texture_Faces_Files[4] = Z_POS;
	m_Texture_Faces_Files[5] = Z_NEG;

	Validate();
}

void TextureCubeMap::SetTextureFaces(const std::string texture_faces[])
{
	for (int i = 0; i < 6; i++)
		m_Texture_Faces_Files[i] = texture_faces[i];

	Validate();
}

void TextureCubeMap::SetTextureFaces(const std::vector<std::string> texture_faces)
{
	for (int i = 0; i < 6; i++)
		m_Texture_Faces_Files[i] = texture_faces[i];

	Validate();
}

void TextureCubeMap::Validate()
{
	if (m_ProgramID > 0)
	{
		glDeleteTextures(1, &m_ProgramID);
		m_ProgramID = 0;
	}

	glGenTextures(1, &m_ProgramID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ProgramID);

	m_InternalFormat = m_Format = GL_RGB;
	for (unsigned int i = 0; i < 6; i++)
	{
		//stbi_set_flip_vertically_on_load(true);
		if (!m_Texture_Faces_Files[i].empty())
		{
			m_Data = (float*)stbi_loadf(m_Texture_Faces_Files[i].c_str(), &m_Width, &m_Height, &m_Channel, 0);
			if (m_Data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, GL_FLOAT, m_Data);
				stbi_image_free(m_Data);
			}
		}
	}
	//stbi_set_flip_vertically_on_load(false);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_ShouldGenerateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (m_ShouldGenerateMipMaps)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	m_IsInitialized = true;
}
