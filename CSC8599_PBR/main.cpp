#include "Renderer.h"

#include <nclgl/Window.h>
#include <nclgl/ProfilingManager.h>

int main()
{
#if _DEBUG
	Window w("CSC8599 - PBR", 1280, 768, false);
#elif NDEBUG
	Window w("CSC8599 - PBR", 1600, 900, true);
#endif

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	bool showPointer = true;
	w.LockMouseToWindow(showPointer);
	w.ShowOSPointer(!showPointer);
	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		ProfilingManager::RecordFrameTimeStart();

		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5))
		{
			Shader::ReloadAllShaders();
		}

		ProfilingManager::RecordFrameTimeEnd();

		/*if (Window::GetMouse()->ButtonDown(MouseButtons::MOUSE_RIGHT))
		{
			showPointer = !showPointer;
			w.ShowOSPointer(showPointer);
		}*/

		//if (ImGui::GetIO().MouseClicked[1])
		//{
		//	showPointer = !showPointer;
		//	ImGui::GetIO().MouseDrawCursor = showPointer;
		//	w.LockMouseToWindow(!showPointer);
		//	/*w.LockMouseToWindow(showPointer);
		//	w.ShowOSPointer(!showPointer);*/
		//}
	}
	return 0;
}