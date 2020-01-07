/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
	............................................________
	....................................,.-‘”...................``~.,
	.............................,.-”...................................“-.,
	.........................,/...............................................”:,
	.....................,?......................................................\,
	.................../...........................................................,}
	................./......................................................,:`^`..}
	.............../...................................................,:”........./
	..............?.....__.........................................:`.........../
	............./__.(.....“~-,_..............................,:`........../
	.........../(_....”~,_........“~,_....................,:`........_/
	..........{.._$;_......”=,_.......“-,_.......,.-~-,},.~”;/....}
	...........((.....*~_.......”=-._......“;,,./`..../”............../
	...,,,___.\`~,......“~.,....................`.....}............../
	............(....`=-,,.......`........................(......;_,,-”
	............/.`~,......`-...............................\....../\
	.............\`~.*-,.....................................|,./.....\,__
	,,_..........}.>-._\...................................|..............`=~-,
	.....`=~-,_\_......`\,.................................\
	...................`=~-,,.\,...............................\
	................................`:,,...........................`\..............__
	.....................................`=-,...................,%`>--==``
	........................................_\..........._,-%.......`\
	...................................,<`.._|_,-&``................`\		
	Win32 Platform Layer
********************************************************************************/

#include<windows.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include<string.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include"typed_platform.h"
#include"typed_maths.h"
#include"typed_render.h"
#include"typed_assets.h"
#include"typed_entity.h"
#include"typed_input.h"
#include"typed_assets.h"
#include"typed.h"

#include"typed_assets.c"
#include"typed_render.c"
#include"typed_entity.c"
#include"typed_input.c"
#include"typed_string.c"
#include"typed_init.c"
#include"typed.c"

#define DEFAULT_FONT_PATH "C:/Windows/Fonts/consola.ttf"

static int64_t globalPerfCount;

struct win32_screen_buffer {	
	int32_t x;
	int32_t y;
	int32_t stride;

	void* data;
	BITMAPINFO info;
};

static void ProcessKeypress(
	union button_state *btnState,
	DWORD isDown)
{
	if(btnState->endedDown != isDown) {
		btnState->endedDown = isDown;
	}
}

extern PLATFORM_ALLOCATE(win32_Allocate)
{
	char *result = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	assert(result);
	return(result);
}

extern PLATFORM_DEALLOCATE(win32_DeAllocate)
{
	VirtualFree(data, 0, MEM_RELEASE);
}

LRESULT CALLBACK WindowProc(
	HWND window,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT result = 0;

	switch (message) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	} case WM_CREATE: {
		struct game_state *state = (struct game_state *)win32_Allocate(sizeof(struct game_state));
		state->characterBuffer = win32_Allocate(KILOBYTES(70));
		SetClassLongPtrW(window, 0, (ULONG_PTR)state);
		break;	
	} case WM_KEYUP: {
		DWORD vkCode = (DWORD)wParam;
		if(vkCode != VK_SHIFT) {break;}
		DWORD wasDown = ((lParam & (1 << 30)) != 0);
		DWORD isDown = ((lParam & (1 << 31)) == 0);		
		
		if(wasDown == isDown) {break;}
		struct game_state *state = (struct game_state *)GetClassLongPtrW(window, 0);
		ProcessKeypress(&state->input.shift, isDown);
		break;
	} case WM_KEYDOWN: {
		DWORD vkCode = (DWORD)wParam;
		DWORD wasDown = ((lParam & (1 << 30)) != 0);
		DWORD isDown = ((lParam & (1 << 31)) == 0);		
		
		if(wasDown == isDown) {break;}
		struct game_state *state = (struct game_state *)GetClassLongPtrW(window, 0);
		
		switch(vkCode) {
		case VK_SHIFT: {
			ProcessKeypress(&state->input.shift, isDown);
			break;
		} case VK_SPACE: {
			state->input.inputCharacter = ' ';
			break;
		} case VK_TAB: {
			BITSET(state->input.state, ISTATE_TAB);
			break;
		} case VK_RETURN: {
			BITSET(state->input.state, ISTATE_RETURN);
			break;
		} case VK_PRIOR: {
			BITSET(state->input.state, ISTATE_PGUP);
			break;
		} case VK_NEXT: {
			BITSET(state->input.state, ISTATE_PGDOWN);
			break;
		} case 0x41: {
			state->input.inputCharacter = state->input.shift.isDown ? 'A' : 'a';
			break;
		} case 0x42: {
			state->input.inputCharacter = state->input.shift.isDown ? 'B' : 'b';
			break;
		} case 0x43: {
			state->input.inputCharacter = state->input.shift.isDown ? 'C' : 'c';
			break;
		} case 0x44: {
			state->input.inputCharacter = state->input.shift.isDown ? 'D' : 'd';
			break;
		} case 0x45: {
			state->input.inputCharacter = state->input.shift.isDown ? 'E' : 'e';
			break;
		} case 0x46: {
			state->input.inputCharacter = state->input.shift.isDown ? 'F' : 'f';
			break;
		} case 0x47: {
			state->input.inputCharacter = state->input.shift.isDown ? 'G' : 'g';
			break;
		} case 0x48: {
			state->input.inputCharacter = state->input.shift.isDown ? 'H' : 'h';
			break;
		} case 0x49: {
			state->input.inputCharacter = state->input.shift.isDown ? 'I' : 'i';
			break;
		} case 0x4A: {
			state->input.inputCharacter = state->input.shift.isDown ? 'J' : 'j';
			break;
		} case 0x4B: {
			state->input.inputCharacter = state->input.shift.isDown ? 'K' : 'k';
			break;
		} case 0x4C: {
			state->input.inputCharacter = state->input.shift.isDown ? 'L' : 'l';
			break;
		} case 0x4D: {
			state->input.inputCharacter = state->input.shift.isDown ? 'M' : 'm';
			break;
		} case 0x4E: {
			state->input.inputCharacter = state->input.shift.isDown ? 'N' : 'n';
			break;
		} case 0x4F: {
			state->input.inputCharacter = state->input.shift.isDown ? 'O' : 'o';
			break;
		} case 0x50: {
			state->input.inputCharacter = state->input.shift.isDown ? 'P' : 'p';
			break;
		} case 0x51: {
			state->input.inputCharacter = state->input.shift.isDown ? 'Q' : 'q';
			break;
		} case 0x52: {
			state->input.inputCharacter = state->input.shift.isDown ? 'R' : 'r';
			break;
		} case 0x53: {
			state->input.inputCharacter = state->input.shift.isDown ? 'S' : 's';
			break;
		} case 0x54: {
			state->input.inputCharacter = state->input.shift.isDown ? 'T' : 't';
			break;
		} case 0x55: {
			state->input.inputCharacter = state->input.shift.isDown ? 'U' : 'u';
			break;
		} case 0x56: {
			state->input.inputCharacter = state->input.shift.isDown ? 'V' : 'v';
			break;
		} case 0x57: {
			state->input.inputCharacter = state->input.shift.isDown ? 'W' : 'w';
			break;
		} case 0x58: {
			state->input.inputCharacter = state->input.shift.isDown ? 'X' : 'x';
			break;
		} case 0x59: {
			state->input.inputCharacter = state->input.shift.isDown ? 'Y' : 'y';
			break;
		} case 0x5A: {
			state->input.inputCharacter = state->input.shift.isDown ? 'Z' : 'z';
			break;
		} case 0xBA: {
			state->input.inputCharacter = state->input.shift.isDown ? ':' : ';';
			break;
		} case 0x31: {
			state->input.inputCharacter = state->input.shift.isDown ? '!' : '1';
			break;
		} case 0x32: {
			state->input.inputCharacter = state->input.shift.isDown ? '"' : '2';
			break;
		} case 0x33: {
			state->input.inputCharacter = state->input.shift.isDown ? '$' : '3';
			break;
		} case 0x34: {
			state->input.inputCharacter = state->input.shift.isDown ? '$' : '4';
			break;
		} case 0x35: {
			state->input.inputCharacter = state->input.shift.isDown ? '%' : '5';
			break;
		} case 0x36: {
			state->input.inputCharacter = state->input.shift.isDown ? '^' : '6';
			break;
		} case 0x37: {
			state->input.inputCharacter = state->input.shift.isDown ? '&' : '7';
			break;
		} case 0x38: {
			state->input.inputCharacter = state->input.shift.isDown ? '*' : '8';
			break;
		} case 0x39: {
			state->input.inputCharacter = state->input.shift.isDown ? '(' : '9';
			break;
		} case 0x30: {
			state->input.inputCharacter = state->input.shift.isDown ? ')' : '0';
			break;
		} case VK_OEM_MINUS: {
			state->input.inputCharacter = state->input.shift.isDown ? '_' : '-';
			break;
		} case VK_OEM_PERIOD: {
			state->input.inputCharacter = state->input.shift.isDown ? '>' : '.';
			break;
		} case VK_OEM_COMMA: {
			state->input.inputCharacter = state->input.shift.isDown ? '<' : ',';
			break;
		} case VK_OEM_2: {
			state->input.inputCharacter = state->input.shift.isDown ? '?' : '/';
			break;
		} case VK_OEM_3: {
			state->input.inputCharacter = state->input.shift.isDown ? '#' : '\'';
			break;
		} case VK_OEM_7: {
			state->input.inputCharacter = state->input.shift.isDown ? '@' : '\'';
			break;
		} default: break;	
		}
		break;
	} default: {
		result = DefWindowProcW(window, message, wParam, lParam);
		break;
	}
	}
	return result;
}

extern PLATFORM_GET_TIME(win32_GetTime)
{
	struct timeval tv;
	static const uint64_t EPOCH = (uint64_t)116444736000000000ULL;

	SYSTEMTIME system_time;
	FILETIME file_time;
	uint64_t time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime );
	time += ((uint64_t)file_time.dwHighDateTime) << 32;
	
	tv.tv_sec  = (int32_t) ((time - EPOCH) / 10000000L);
	tv.tv_usec = (int32_t) (system_time.wMilliseconds * 1000);	
	
	return((uint64_t) tv.tv_sec * (uint64_t)1000000UL + (uint64_t)tv.tv_usec);	
}

extern PLATFORM_READ_FILE(win32_ReadFile)
{
	struct file_read_output result = {};
	HANDLE handle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ,
		0, OPEN_EXISTING, 0, 0);
	assert(handle != INVALID_HANDLE_VALUE);
	LARGE_INTEGER size;
	if (GetFileSizeEx(handle, &size)) {
		uint32_t size32 = size.QuadPart;
		result.contents = VirtualAlloc(0, size32,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		assert(result.contents);
		DWORD bytesRead;
		if (ReadFile(handle, result.contents, size32, &bytesRead, 0) &&
			(size32 == bytesRead)) {
			result.contentsSize = size32;
		}
		else {
			VirtualFree(result.contents, 0, MEM_RELEASE);
			result.contents = 0;
		}
	}
	return(result);
}

static void OutputScreenBuffer(
	struct win32_screen_buffer* b,
	HDC dc)
{
	StretchDIBits(dc, 0, 0, b->x, b->y,
		0, 0, b->x, b->y,
		b->data, &b->info,
		DIB_RGB_COLORS, SRCCOPY);
}

static void InitScreenBuffer(
	const int32_t x,
	const int32_t y,
	struct win32_screen_buffer* b)
{
	b->x = x;
	b->y = y;
	b->stride = x * BYTES_PER_PIXEL;

	b->info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	b->info.bmiHeader.biWidth = x;
	b->info.bmiHeader.biHeight = y;
	b->info.bmiHeader.biPlanes = 1;
	b->info.bmiHeader.biBitCount = 32;
	b->info.bmiHeader.biCompression = BI_RGB;
	size_t bufferSize = (x * y) * BYTES_PER_PIXEL;
	b->data = VirtualAlloc(0, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

static inline float win32_GetSecondsElapsed(
	LARGE_INTEGER start,
	LARGE_INTEGER end) 
{
	float result = ((float)(end.QuadPart - start.QuadPart) /
			(float)globalPerfCount);
	return(result);				
}

static inline LARGE_INTEGER win32_GetWallClock(void)
{
	LARGE_INTEGER result;
	QueryPerformanceCounter(&result);
	return(result);
} 

static void LimitFps(
	LARGE_INTEGER lastCounter,
	bool timeGranularity)
{
#define TARGET_FPS 60
#define TARGET_SPF (1.0f / TARGET_FPS)	
	LARGE_INTEGER workCounter = win32_GetWallClock();
	float secondsElapsedForFrame = win32_GetSecondsElapsed(lastCounter, workCounter);			
	if(secondsElapsedForFrame < TARGET_SPF) {
		DWORD sleepMS = (DWORD)(1000.0f * (TARGET_SPF - secondsElapsedForFrame));
		if(timeGranularity) {
			if(sleepMS > 0.0f) 
				Sleep(sleepMS);
		}						
		while(secondsElapsedForFrame < TARGET_SPF) {
			secondsElapsedForFrame = win32_GetSecondsElapsed(lastCounter, 
				win32_GetWallClock());									
		}
	}
}

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow)
{
	WNDCLASSEXW winClass = {};
	winClass.cbSize = sizeof(WNDCLASSEXW);
	winClass.style = CS_OWNDC;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = sizeof(ULONG_PTR);
	winClass.hInstance = hInstance;
	winClass.lpszClassName = L"mainclass";
	RegisterClassExW(&winClass);

	HWND window = CreateWindowExW(0,
		winClass.lpszClassName, L"typed",
		WS_OVERLAPPED  | WS_VISIBLE | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 980, 420,
		NULL, NULL, hInstance, NULL);
		
	struct game_state *state = (struct game_state *)GetClassLongPtrW(window, 0);

	api.ReadFile = win32_ReadFile;
	api.GetTime = win32_GetTime;
	api.Allocate = win32_Allocate;
	api.DeAllocate = win32_DeAllocate;

	RECT clientRect;
	GetClientRect(window, &clientRect);
	struct win32_screen_buffer winBuffer = {};
	InitScreenBuffer(
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top,
		&winBuffer);
	bufferX = winBuffer.x;	
	bufferY = winBuffer.y;
	
	struct screen_buffer buffer = {};
	buffer.x = winBuffer.x;	
	buffer.y = winBuffer.y;	
	buffer.stride = winBuffer.stride;
	
	bool timeGranularity = (timeBeginPeriod(1) == TIMERR_NOERROR);		
	
	LARGE_INTEGER lastCounter = win32_GetWallClock();
	LARGE_INTEGER perfCount;
	QueryPerformanceFrequency(&perfCount);	
	globalPerfCount = perfCount.QuadPart;

	LoadFont(state->characterBuffer, DEFAULT_FONT_PATH);	
	
	MSG msg;
	while(1) {
		if(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {break;}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		buffer.data = winBuffer.data;
		
		LimitFps(lastCounter, timeGranularity);
		
		GameLoop(state, &buffer);
		
		HDC dc = GetDC(window);
		OutputScreenBuffer(&winBuffer, dc);
		ReleaseDC(window, dc);
		
		lastCounter = win32_GetWallClock();
	}
}