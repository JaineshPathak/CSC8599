#include "ProfilingManager.h"

int ProfilingManager::DrawCalls = 0;
int ProfilingManager::TrianglesCount = 0;
int ProfilingManager::VerticesCount = 0;
int ProfilingManager::TrianglesCountCurrent = 0;
int ProfilingManager::VerticesCountCurrent = 0;

double ProfilingManager::m_StartupTimeSec = 0.0;
double ProfilingManager::m_TextureLoadTimeSec = 0.0;
double ProfilingManager::m_FrameTimeSec = 0.0;

std::chrono::high_resolution_clock::time_point ProfilingManager::m_StartupStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_StartupEndTime = std::chrono::high_resolution_clock::now();

std::chrono::high_resolution_clock::time_point ProfilingManager::m_TextureStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_TextureEndTime = std::chrono::high_resolution_clock::now();

std::chrono::high_resolution_clock::time_point ProfilingManager::m_FrameStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_FrameEndTime = std::chrono::high_resolution_clock::now();

float ProfilingManager::GetFramerate()
{
	if (ImGui::GetCurrentContext() == nullptr)
		return 0;

	return ImGui::GetIO().Framerate;
}

void ProfilingManager::RecordStartupTimeStart()
{
	m_StartupStartTime = std::chrono::high_resolution_clock::now();
}

void ProfilingManager::RecordStartupTimeEnd()
{
	m_StartupEndTime = std::chrono::high_resolution_clock::now();
	m_StartupTimeSec = std::chrono::duration_cast<std::chrono::milliseconds>(m_StartupEndTime - m_StartupStartTime).count() / 1000.0;
}

void ProfilingManager::RecordTextureTimeStart()
{
	m_TextureStartTime = std::chrono::high_resolution_clock::now();
}

void ProfilingManager::RecordTextureTimeEnd()
{
	m_TextureEndTime = std::chrono::high_resolution_clock::now();
	m_TextureLoadTimeSec = std::chrono::duration_cast<std::chrono::milliseconds>(m_TextureEndTime - m_TextureStartTime).count() / 1000.0;
}

void ProfilingManager::RecordFrameTimeStart()
{
	m_FrameStartTime = std::chrono::high_resolution_clock::now();
}

void ProfilingManager::RecordFrameTimeEnd()
{
	m_FrameEndTime = std::chrono::high_resolution_clock::now();
	m_FrameTimeSec = std::chrono::duration_cast<std::chrono::milliseconds>(m_FrameEndTime - m_FrameStartTime).count() / 1000.0;
}
