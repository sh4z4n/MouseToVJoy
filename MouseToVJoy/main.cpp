#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "fileRead.h"
#include "vjoy.h"
#include "mousetovjoy.h"
#include "input.h"



const char g_szClassName[] = "myWindowClass";
HWND hwnd;
WNDCLASSEX wc;
MSG Msg;
vJoy vJ;
mouseToVjoy mTV;
cInputDevices rInput;
fileRead fR;
INT X, Y, Z, RX;
BOOL BUTTON1, BUTTON2, BUTTON3;
string keyCodeName;

using namespace std;
void InitializationCode() {
	//Code that is run only once, tests vjoy device, reads config file and prints basic out accuired vars.
	vJ.testDriver();
	vJ.testVirtualDevices(1);
	vJ.accuireDevice(1);
	string fileName = "config.txt";
	string checkArray[21] = {"Sensitivity", "AttackTimeThrottle", "ReleaseTimeThrottle", "AttackTimeBreak", "ReleaseTimeBreak", "AttackTimeClutch", "ReleaseTimeClutch", "ThrottleKey", "BreakKey", "ClutchKey", "GearShiftUpKey", "GearShiftDownKey", "HandBrakeKey", "MouseLockKey", "MouseCenterKey", "UseMouse","UseCenterReduction" , "AccelerationThrottle", "AccelerationBreak", "AccelerationClutch", "CenterMultiplier"};
	fR.newFile(fileName, checkArray);

	printf("==================================\n");
	printf("Sensitivity = %.2f \n",fR.result(0));
	printf("Throttle Attack Time = %.0f \n", fR.result(1));
	printf("Throttle Release Time = %.0f \n", fR.result(2));
	printf("Break Attack Time = %.0f \n", fR.result(3));
	printf("Break Release Time = %.0f \n", fR.result(4));
	printf("Clutch Attack Time = %.0f \n", fR.result(5));
	printf("Clutch Release Time = %.0f \n", fR.result(6));
	printf("Throttle key = %d \n", (int)fR.result(7));
	printf("Break key = %d \n", (int)fR.result(8));
	printf("Clutch key = %d \n", (int)fR.result(9));
	printf("Gear Shift Up key = %d \n", (int)fR.result(10));
	printf("Gear Shift Down key = %d \n", (int)fR.result(11));
	printf("Hand Brake Key = %d \n", (int)fR.result(12));
	printf("Mouse Lock key = %d \n", (int)fR.result(13));
	printf("Mouse Center key = %d \n", (int)fR.result(14));
	printf("Use Mouse = %d \n", (int)fR.result(15));
	printf("Use Center Reduction = %d \n", (int)fR.result(16));
	printf("Acceleration Throttle = %.2f \n", fR.result(17));
	printf("Acceleration Break = %.2f \n", fR.result(18));
	printf("Acceleration Clutch = %.2f \n", fR.result(19));
	printf("Center Multiplier = %.2f \n", fR.result(20));
	printf("==================================\n");
}
void UpdateCode() {
	//Code that is run every time program gets an message from enviroment(mouse movement, mouse click etc.), manages input logic and feeding device.
	mTV.inputLogic(rInput, X, Y, Z, RX, BUTTON1, BUTTON2, BUTTON3, fR.result(1), fR.result(2), fR.result(3), fR.result(4), fR.result(5), fR.result(6), fR.result(7), fR.result(8), fR.result(9), fR.result(10), fR.result(11), fR.result(12), fR.result(13), fR.result(14),fR.result(16), fR.result(17), fR.result(18), fR.result(19));
	vJ.feedDevice(1, X, Y, Z, RX, BUTTON1, BUTTON2, BUTTON3);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
		//Creating new twi raw input devices
			RAWINPUTDEVICE m_Rid[2];
			//Keyboard
			m_Rid[0].usUsagePage = 1;
			m_Rid[0].usUsage = 6;
			m_Rid[0].dwFlags = RIDEV_INPUTSINK;
			m_Rid[0].hwndTarget = hwnd;
			// Mouse
			m_Rid[1].usUsagePage = 1;
			m_Rid[1].usUsage = 2;
			m_Rid[1].dwFlags = RIDEV_INPUTSINK;
			m_Rid[1].hwndTarget = hwnd;
			RegisterRawInputDevices(m_Rid, 2, sizeof(RAWINPUTDEVICE));

		break;
	case WM_INPUT:
		//Then window recives input message get data for rinput device and run mouse logic function.
			rInput.GetData(lParam);
			mTV.mouseLogic(rInput, X, fR.result(0), fR.result(20), fR.result(16));
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
	case WM_DESTROY:
		DestroyWindow(hwnd);
	default:
		return DefWindowProc(hwnd, Msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//invisible window initialization to be able to recive raw input even if the window is not focused.
	static const char* class_name = "DUMMY_CLASS";
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;        // function which will handle messages
	wc.hInstance = hInstance;
	wc.lpszClassName = class_name;
	if (RegisterClassEx(&wc)) {
		CreateWindowEx(0, class_name, "dummy_name", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
	}
	//Allocating console to process and redirect every stdout, stdin to it.
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	ios::sync_with_stdio();
	//Show invisible window, update it, then do initialization code.
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	InitializationCode();
	

	//Loop on PeekMessage instead of GetMessage to avoid overflow.
	while (true) {
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		//To optimalize cpu usade wait 2 milisecond before running update code.
		Sleep(2);
		UpdateCode();
		//	if (BUTTON1) printf("btn1\n"); DEBUG
		//	if (BUTTON2) printf("btn2\n"); DEBUG
		//If Message is equal to quit then break loop and end program.
		if (Msg.message == WM_QUIT || Msg.message == WM_DESTROY)
		{
			break;
		}

		}
		
	}

