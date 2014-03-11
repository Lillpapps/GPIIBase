// Utils.h

#pragma once

class Timer {
	unsigned __int64 m_freqency;
	unsigned __int64 m_current;
	unsigned __int64 m_previous;
	double m_deltatime;
public:
	Timer();
	float Update();
};

class File {
public:
	static bool Exists(const char* filename);
	static unsigned int Size(const char* filename);
	static void Read(const char* filename, void* buffer, unsigned int &size);
};

class Debug {
public:
	static void Msg(const char *fmt, ...);
};
