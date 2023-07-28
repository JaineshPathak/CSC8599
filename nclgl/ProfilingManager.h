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

	static float GetFramerate();
	static double GetStartupTime() { return m_StartupTimeSec; }
	static double GetTextureLoadTime() { return m_TextureLoadTimeSec; }
	static double GetFrameTime() { return m_FrameTimeSec; }
	static double GetSkyboxCaptureTime() { return m_SkyboxCaptureTimeSec; }

	static int DrawCalls;
	static int TrianglesCount;
	static int VerticesCount;

	static int TrianglesCountCurrent;
	static int VerticesCountCurrent;

private:
	static double m_StartupTimeSec;
	static double m_TextureLoadTimeSec;
	static double m_FrameTimeSec;
	static double m_SkyboxCaptureTimeSec;

	static std::chrono::high_resolution_clock::time_point m_StartupStartTime, m_StartupEndTime;
	static std::chrono::high_resolution_clock::time_point m_TextureStartTime, m_TextureEndTime;
	static std::chrono::high_resolution_clock::time_point m_FrameStartTime, m_FrameEndTime;
	static std::chrono::high_resolution_clock::time_point m_SkyboxCaptureStartTime, m_SkyboxCaptureEndTime;
};