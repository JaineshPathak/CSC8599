#pragma once
#include "Texture.h"
#include <vector>

class TextureCubeMap : public Texture
{
public:
	TextureCubeMap(const unsigned int width, const unsigned int height, bool generateMipMaps = false);
	TextureCubeMap(const std::string& X_POS, const std::string& X_NEG,
					const std::string& Y_POS, const std::string& Y_NEG,
					const std::string& Z_POS, const std::string& Z_NEG, bool generateMipmaps = false);
	TextureCubeMap(const std::string texture_faces[], bool generateMipmaps = false);
	TextureCubeMap(const std::vector<std::string> texture_faces, bool generateMipmaps = false);

	virtual void SetTextureFaces(const std::string& X_POS, const std::string& X_NEG,
								const std::string& Y_POS, const std::string& Y_NEG,
								const std::string& Z_POS, const std::string& Z_NEG);
	virtual void SetTextureFaces(const std::string texture_faces[]);
	virtual void SetTextureFaces(const std::vector<std::string> texture_faces);

protected:
	virtual void Validate() override;
	
	std::string m_Texture_Faces_Files[6];
	bool m_ShouldGenerateMipMaps;
};