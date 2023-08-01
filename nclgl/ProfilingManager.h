#pragma once
#include <imgui/imgui_internal.h>

#include <Windows.h>
#include <psapi.h>
#include <chrono>
#include <string>

class ProfilingManager
{
public:
	static void RecordStartupTimeStart();
	static void RecordStartupTimeEnd();

	static void RecordTextureTimeStart();
	static void RecordTextureTimeEnd();

	static void RecordFrameTimeStart();
	static void RecordFrameTimeEnd();

	static void RecordSkyboxCaptureTimeStart();
	static void RecordSkyboxCaptureTimeEnd();

	static void RecordPostProcessTimeStart();
	static void RecordPostProcessTimeEnd();

	static const float GetFramerate();
	static const double GetStartupTime() { return m_StartupTimeSec; }
	static const double GetTextureLoadTime() { return m_TextureLoadTimeSec; }
	static const double GetFrameTime() { return m_FrameTimeSec; }
	static const double GetSkyboxCaptureTime() { return m_SkyboxCaptureTimeSec; }
	static const double GetPostProcessTime() { return m_PostProcessTimeSec; }

	static const std::string GetVirtualMemoryUsage() { return ConvertMemoryUsage(m_UsedVirtualMem); }
	static const std::string GetVirutalUsageByProgram() { return ConvertMemoryUsage(m_VirtualMemUsedByProgram); }
	static const std::string GetTotalVirtualMemory() { return ConvertMemoryUsage(m_TotalVirtualMem); }
	static const std::string GetPhysicalMemoryUsage() { return ConvertMemoryUsage(m_UsedPhysMem); }
	static const std::string GetPhysicalUsagebyProgram() { return ConvertMemoryUsage(m_PhysMemUsedByProgram); }
	static const std::string GetTotalPhysicalMemory() { return ConvertMemoryUsage(m_TotalPhysMem); }
	
	static void Update();

	static long DrawCalls;
	static long long TrianglesCount;
	static long long VerticesCount;

	static long long TrianglesCountCurrent;
	static long long VerticesCountCurrent;

private:
	static double m_StartupTimeSec;
	static double m_TextureLoadTimeSec;
	static double m_FrameTimeSec;
	static double m_SkyboxCaptureTimeSec;
	static double m_PostProcessTimeSec;

	static const int byteToMb = 1048576;
	static std::string ConvertMemoryUsage(DWORDLONG a) { return std::to_string(a / byteToMb) + " MB"; }

	static std::chrono::high_resolution_clock::time_point m_StartupStartTime, m_StartupEndTime;
	static std::chrono::high_resolution_clock::time_point m_TextureStartTime, m_TextureEndTime;
	static std::chrono::high_resolution_clock::time_point m_FrameStartTime, m_FrameEndTime;
	static std::chrono::high_resolution_clock::time_point m_SkyboxCaptureStartTime, m_SkyboxCaptureEndTime;
	static std::chrono::high_resolution_clock::time_point m_PostProcessStartTime, m_PostProcessEndTime;
	
	static void CalculateMemoryUsage();
	static void CalculateMemoryUsageByProgram();

	static DWORDLONG	m_TotalVirtualMem;
	static DWORDLONG	m_UsedVirtualMem;
	static SIZE_T		m_VirtualMemUsedByProgram;

	static DWORDLONG	m_TotalPhysMem;
	static DWORDLONG	m_UsedPhysMem;
	static SIZE_T		m_PhysMemUsedByProgram;
};