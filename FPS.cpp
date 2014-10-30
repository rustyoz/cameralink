#include "FPS.h"


Fps::Fps() : m_fps(0), initial_(GetTickCount())
	{
};



void Fps::update()
{
	m_fps = 1000 / (float)(GetTickCount() - initial_);
	initial_ = GetTickCount();
}

	// Get fps
float Fps::get() 
	{
		
		m_fps = 1000.0 / (float)(GetTickCount() - initial_);
		initial_ = GetTickCount();
		return m_fps;
};


