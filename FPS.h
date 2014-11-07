class Fps
{
private:
	float m_fps;
	unsigned int m_fpscount;
	unsigned int initial_;
	inline unsigned int value() const;

public:
	// Constructor
	Fps();

	// Update
	void update();

	// Get fps
	float get();

	

};
