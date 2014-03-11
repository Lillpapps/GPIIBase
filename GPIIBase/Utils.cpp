// Utils.cpp

#include "stdafx.h"
#include <cstdarg>
#include <sys\stat.h>
#include "Utils.h"

Timer::Timer() {
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freqency);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_current);
	m_previous = m_current - 10000;
	m_deltatime = (double)(m_current - m_previous) / (double)m_freqency;
};

float Timer::Update() {
	QueryPerformanceCounter((LARGE_INTEGER*)&m_current);
	unsigned __int64 delta = m_current - m_previous;
	m_deltatime = (double)(m_current - m_previous) / (double)m_freqency;
	m_previous = m_current;
	return (float)m_deltatime;
};

bool File::Exists(const char* filename) {
	DWORD attr = GetFileAttributesA(filename);
	return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
};

unsigned int File::Size(const char* filename) {
	struct stat s;
	int rc = stat(filename, &s);
	return rc == 0 ? s.st_size : 0;
};

void File::Read(const char* filename, void* buffer, unsigned int &size) {
	std::ifstream stream(filename, std::ios_base::in | std::ios_base::binary);
	stream.read((char*)buffer, size);
	stream.close();
	((char*)buffer)[size] = 0;
};

void Debug::Msg(const char *fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	char buffer[2048];
	vsprintf_s(buffer, sizeof(buffer), fmt, vl);
	va_end(vl);
	MessageBoxA(NULL, buffer, "Debug::Message!", MB_OK|MB_ICONEXCLAMATION);
};
