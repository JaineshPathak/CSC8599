#pragma once
#include <imgui/imgui_internal.h>
#include <chrono>

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

	static std::chrono::high_resolution_clock::time_point m_StartupStartTime, m_StartupEndTime;
	static std::chrono::high_resolution_clock::time_point m_TextureStartTime, m_TextureEndTime;
	static std::chrono::high_resolution_clock::time_point m_FrameStartTime, m_FrameEndTime;
	static std::chrono::high_resolution_clock::time_point m_SkyboxCaptureStartTime, m_SkyboxCaptureEndTime;
	static std::chrono::high_resolution_clock::time_point m_PostProcessStartTime, m_PostProcessEndTime;
};