//    //
//  WindowManager.cpp    //  GLApplication.cpp
//  OpenGL    //  OpenGL
//    //
//  Created by xt on 15/8/10.    //  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.    //  Copyright (c) 2015年 xt. All rights reserved.
//    //


#include "windowManager.h"
#include "inputmanager.h"
#include "camera.h"
#include "sys.h"
#include "ad.h"
#include "gli.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl.h"
#include "imgui/imgui_impl_opengl3.h"

extern void  mouse_button_callback(GLUwindow* window, int button, int action, int mods);
extern void mouse_curse_pos_callback(GLUwindow *window, double xpos, double ypos);
extern void  mouse_scroll_callback(GLUwindow *window, double xoffse, double yoffse);
extern void  key_callback(GLUwindow*window, int key, int st, int action, int mods);
extern void windowSize_callback(GLUwindow*window, int width, int height);


WindowManager::WindowManager() :inputManager_(nullptr),
window_(nullptr)
{
}


WindowManager::~WindowManager()
{
	destroy();
}

bool WindowManager::initialize()
{
	if (!gluInitWindow()) return false;

	return true;
}


bool WindowManager::createWindow(int width, int height, const char *title)
{
	window_ = gluCreateWindow(width, height, title);
	if (!window_) return false;

	gluSetKeyCallback(window_, key_callback);
	gluSetMouseButtonCallback(window_, mouse_button_callback);
	gluSetCursorPosCallback(window_, mouse_curse_pos_callback);
	gluSetScrollCallback(window_, mouse_scroll_callback);
	gluSetWindowSizeCallback(window_, windowSize_callback);
	//g_main_window = window_;

	gluMakeCurrentContext(window_);

	gluSwapInterval(1);

	if (!GL_X::initGlew()) return false;

	GL_X::testOpenGl();

	/*if (dc->glContext_.debug_context_)
	if (GL_X::isSupportExtension("GL_ARB_debug_output"))
	GL_X::init_opengl_dbg_win(dc->glContext_.debug_sync_);*/

	inputManager_ = InputManager::instance();
	inputManager_->setWindow(window_);

	return true;
}


bool WindowManager::initGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char* glsl_version = "#version 130";
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	return true;
}

bool WindowManager::destoryGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return true;
}


void WindowManager::guiFrameBegin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void WindowManager::guiFrameRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void WindowManager::loop()
{

}

void WindowManager::destroy()
{
	if (inputManager_)
	{
		delete inputManager_;
		inputManager_ = NULL;
	}
}

GLUwindow* WindowManager::getWindow()
{
	return window_;
}

bool WindowManager::shouldeClose()const
{
	if (window_) return gluWindowShouldClose(window_);
	return true;
}

bool WindowManager::processInput(bool continueGame = true)
{
	return continueGame;
}

InputManager* WindowManager::getInputManager()
{
	return  inputManager_;
}

void WindowManager::swapTheBuffers()
{
	if (window_) gluSwapBuffer(window_);
}

