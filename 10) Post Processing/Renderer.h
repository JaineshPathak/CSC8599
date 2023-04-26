#pragma once
#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\Camera.h"
#include "..\nclgl\HeightMap.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	Shader* sceneShader;
	Shader* processShader;

	Camera* camera;

	Mesh* quad;
	HeightMap* heightMap;
	GLuint heightTexture;

	GLuint bufferFBO;
	GLuint processFBO;

	GLuint bufferColorTex[2];
	GLuint bufferDepthTex;

	float dTime;
	GameTimer* timer;
};