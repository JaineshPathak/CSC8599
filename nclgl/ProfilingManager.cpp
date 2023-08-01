#include "ProfilingManager.h"

long ProfilingManager::DrawCalls = 0;
long long ProfilingManager::TrianglesCount = 0;
long long ProfilingManager::VerticesCount = 0;
long long ProfilingManager::TrianglesCountCurrent = 0;
long long ProfilingManager::VerticesCountCurrent = 0;

double ProfilingManager::m_StartupTimeSec = 0.0;
double ProfilingManager::m_TextureLoadTimeSec = 0.0;
double ProfilingManager::m_FrameTimeSec = 0.0;
double ProfilingManager::m_SkyboxCaptureTimeSec = 0.0;
double ProfilingManager::m_PostProcessTimeSec = 0.0;

std::chrono::high_resolution_clock::time_point ProfilingManager::m_StartupStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_StartupEndTime = std::chrono::high_resolution_clock::now();

std::chrono::high_resolution_clock::time_point ProfilingManager::m_TextureStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_TextureEndTime = std::chrono::high_resolution_clock::now();

std::chrono::high_resolution_clock::time_point ProfilingManager::m_FrameStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_FrameEndTime = std::chrono::high_resolution_clock::now();

std::chrono::high_resolution_clock::time_point ProfilingManager::m_SkyboxCaptureStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_SkyboxCaptureEndTime = std::chrono::high_resolution_clock::now();

std::chrono::high_resolution_clock::time_point ProfilingManager::m_PostProcessStartTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point ProfilingManager::m_PostProcessEndTime = std::chrono::high_resolution_clock::now();

DWORDLONG ProfilingManager::m_TotalVirtualMem = 0;
DWORDLONG ProfilingManager::m_UsedVirtualMem = 0;
SIZE_T ProfilingManager::m_VirtualMemUsedByProgram = 0;

DWORDLONG ProfilingManager::m_TotalPhysMem = 0;
DWORDLONG ProfilingManager::m_UsedPhysMem = 0;
SIZE_T ProfilingManager::m_PhysMemUsedByProgram = 0;

const float ProfilingManager::GetFramerate()
{
	if (ImGui::GetCurrentContext() == nullptr)
		return 0;

	return (const float)ImGui::GetIO().Framerate;
}

void ProfilingManager::Update()
{
	CalculateMemoryUsage();
	CalculateMemoryUsageByProgram();
}

void ProfilingManager::CalculateMemoryUsage()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	m_TotalVirtualMem = memInfo.ullTotalPageFile;
	m_UsedVirtualMem = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;

	m_TotalPhysMem = memInfo.ullTotalPhys;
	m_UsedPhysMem = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
}

void ProfilingManager::CalculateMemoryUsageByProgram()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

	m_VirtualMemUsedByProgram = pmc.PrivateUsage;
	m_PhysMemUsedByProgram = pmc.WorkingSetSize;
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

void ProfilingManager::RecordSkyboxCaptureTimeStart()
{
	m_SkyboxCaptureStartTime = std::chrono::high_resolution_clock::now();
}

void ProfilingManager::RecordSkyboxCaptureTimeEnd()
{
	m_SkyboxCaptureEndTime = std::chrono::high_resolution_clock::now();
	m_SkyboxCaptureTimeSec = std::chrono::duration_cast<std::chrono::milliseconds>(m_SkyboxCaptureEndTime - m_SkyboxCaptureStartTime).count() / 1000.0;
}

void ProfilingManager::RecordPostProcessTimeStart()
{
	m_PostProcessStartTime = std::chrono::high_resolution_clock::now();
}

void ProfilingManager::RecordPostProcessTimeEnd()
{
	m_PostProcessEndTime = std::chrono::high_resolution_clock::now();
	m_PostProcessTimeSec = std::chrono::duration_cast<std::chrono::milliseconds>(m_PostProcessEndTime - m_PostProcessStartTime).count() / 1000.0;
}
