#pragma once
#include "TextureCubeMap.h"

class TextureEnvCubeMap : public TextureCubeMap
{
public:
	TextureEnvCubeMap(const unsigned int width, const unsigned int height, bool generateMipMaps = false);
	virtual void SetTextureFaces(const std::vector<std::string> texture_faces) override;

protected:
	virtual void Validate() override;
};