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

	static float GetFramerate();
	static double GetStartupTime() { return m_StartupTimeSec; }
	static double GetTextureLoadTime() { return m_TextureLoadTimeSec; }
	static double GetFrameTime() { return m_FrameTimeSec; }

	static int DrawCalls;
	static int TrianglesCount;
	static int VerticesCount;

	static int TrianglesCountCurrent;
	static int VerticesCountCurrent;

private:
	static double m_StartupTimeSec;
	static double m_TextureLoadTimeSec;
	static double m_FrameTimeSec;

	static std::chrono::high_resolution_clock::time_point m_StartupStartTime, m_StartupEndTime;
	static std::chrono::high_resolution_clock::time_point m_TextureStartTime, m_TextureEndTime;
	static std::chrono::high_resolution_clock::time_point m_FrameStartTime, m_FrameEndTime;
};