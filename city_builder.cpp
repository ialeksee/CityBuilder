#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine/olcPixelGameEngine.h"
#include <cstdio>

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPixelGameEngine/extensions/olcPGEX_TransformedView.h"

#include "olcPixelGameEngine/utilities/olcUTIL_Camera2D.h"

#define cTileSize 16
#define cScreenWidth 640
#define cScreenHeight 480

class MainWindow : public olc::PixelGameEngine
{
    public:
        MainWindow() { sAppName = "CityBuilder";}
		// Transformed view object to make world offsetting simple

	private:
		std::unique_ptr<olc::Sprite> pTexturedGrassTile;
		std::unique_ptr<olc::Sprite> pDeadGrassTile;
		std::unique_ptr<olc::Sprite> pGrassTile;
	   // The world map, stored as a 1D array
	   std::vector<uint8_t> vWorldMap;
	
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	void DrawTile(int x, int y, uint8_t tileConst);
	olc::vi2d vBlockSize = { 16,16 };
};

bool MainWindow::OnUserCreate()
{
	// Called once at the start, so create things here
	pTexturedGrassTile = std::make_unique<olc::Sprite>("./Sprites/Ground/TexturedGrass.png");
	pDeadGrassTile = std::make_unique<olc::Sprite>("./Sprites/Ground/DeadGrass.png");
	pGrassTile = std::make_unique<olc::Sprite>("./Sprites/Ground/Grass.png");

	//16x16 pixels tile; 800x480 window
	vWorldMap.resize(40*30);

	for(int i = 0; i < vWorldMap.size(); i++)
	{
		vWorldMap[i] = (rand() % cTileSize) + 1;	
	}

	return true;			
}

bool MainWindow::OnUserUpdate(float fElapsedTime)
{
		int x = 0;
		int y = 0;
	//draw the map
	for(int i = 0; i < vWorldMap.size(); i++)
	{
		DrawTile(x, y, vWorldMap[i]);
		x += cTileSize;
		if(x >= cScreenWidth)
		{
			x = 0;
			y += cTileSize;
		}
	}


	return true;
}

void MainWindow::DrawTile(int x, int y, uint8_t tileConst)
{
	switch(tileConst)
	{
		case 1:	
			DrawPartialSprite(olc::vi2d(x, y), pTexturedGrassTile.get(), olc::vi2d(0, 0) * vBlockSize, vBlockSize);
			break;
		case 2:	
			DrawPartialSprite(olc::vi2d(x, y), pTexturedGrassTile.get(), olc::vi2d(1, 0) * vBlockSize, vBlockSize);
			break;
		case 3:	
			DrawPartialSprite(olc::vi2d(x, y), pTexturedGrassTile.get(), olc::vi2d(2, 0) * vBlockSize, vBlockSize);
			break;
		case 4:	
			DrawPartialSprite(olc::vi2d(x, y), pTexturedGrassTile.get(), olc::vi2d(0, 1) * vBlockSize, vBlockSize);
			break;
		case 5:	
			DrawPartialSprite(olc::vi2d(x, y), pTexturedGrassTile.get(), olc::vi2d(1, 1) * vBlockSize, vBlockSize);
			break;
		case 6:	
			DrawPartialSprite(olc::vi2d(x, y), pTexturedGrassTile.get(), olc::vi2d(2, 1) * vBlockSize, vBlockSize);
			break;
		case 7:	
			DrawPartialSprite(olc::vi2d(x, y), pDeadGrassTile.get(), olc::vi2d(0, 0) * vBlockSize, vBlockSize);
			break;
		case 8:	
			DrawPartialSprite(olc::vi2d(x, y), pDeadGrassTile.get(), olc::vi2d(1, 0) * vBlockSize, vBlockSize);
			break;
		case 9:	
			DrawPartialSprite(olc::vi2d(x, y), pDeadGrassTile.get(), olc::vi2d(2, 0) * vBlockSize, vBlockSize);
			break;
		case 10:	
			DrawPartialSprite(olc::vi2d(x, y), pDeadGrassTile.get(), olc::vi2d(0, 1) * vBlockSize, vBlockSize);
			break;
		case 11:	
			DrawPartialSprite(olc::vi2d(x, y), pDeadGrassTile.get(), olc::vi2d(1, 1) * vBlockSize, vBlockSize);
			break;
		case 12:	
			DrawPartialSprite(olc::vi2d(x, y), pDeadGrassTile.get(), olc::vi2d(2, 1) * vBlockSize, vBlockSize);
			break;
		case 13:	
			DrawPartialSprite(olc::vi2d(x, y), pGrassTile.get(), olc::vi2d(1, 0) * vBlockSize, vBlockSize);
			break;
		case 14:	
			DrawPartialSprite(olc::vi2d(x, y), pGrassTile.get(), olc::vi2d(2, 0) * vBlockSize, vBlockSize);
			break;
		case 15:	
			DrawPartialSprite(olc::vi2d(x, y), pGrassTile.get(), olc::vi2d(3, 0) * vBlockSize, vBlockSize);
			break;
		case 16:	
			DrawPartialSprite(olc::vi2d(x, y), pGrassTile.get(), olc::vi2d(4, 0) * vBlockSize, vBlockSize);
			break;
	}
}

int main()
{
    MainWindow wnd;
    wnd.Construct(cScreenWidth, cScreenHeight, 2, 2);
    wnd.Start();

    return 0;
}