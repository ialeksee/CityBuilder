#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine/olcPixelGameEngine.h"
#include <cstdio>

class MainWindow : public olc::PixelGameEngine
{
    public:
        MainWindow() { sAppName = "CityBuilder";}
	
    bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};


int main()
{
    MainWindow wnd;
    wnd.Construct(780, 480, 2, 2);
    wnd.Start();

    return 0;
}