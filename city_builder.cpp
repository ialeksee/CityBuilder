#include <cassert>
#include <cstdint>
#include <filesystem>
#include <array>
#include <fstream>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine/olcPixelGameEngine.h"
#include <cstdio>

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPixelGameEngine/extensions/olcPGEX_TransformedView.h"

#include "olcPixelGameEngine/utilities/olcUTIL_Camera2D.h"

constexpr uint8_t  cTileSize{16};
constexpr uint32_t cScreenWidth{640};
constexpr uint32_t cScreenHeight{480};

using namespace olc;
constexpr uint32_t cMapWidth{2048};
constexpr uint32_t cMapHeight{1024};
//Set the world at 160x160 16 pixel wide square tiles
constexpr uint32_t cWorldSize{160};
constexpr uint32_t cViewPortWidthTiles{40};
constexpr uint32_t cViewPortHeightTiles{30};
constexpr int32_t cHousePrice{10};

int32_t money{0};
uint32_t population{0};
uint32_t numberOfHouses{0};

enum class MenuItem {house, none};

class Menu{
	olc::vi2d pos;
	olc::vi2d size;

	public:
	Menu() : pos({60,370}), size({480,80}){};
	olc::vi2d getOrigin(){return pos;};
	olc::vi2d getSize(){return size;};
	MenuItem getMenuItem(const olc::vi2d& cursorPos);
	bool isPositionInside(const olc::vi2d& cursorPos);
};

MenuItem Menu::getMenuItem(const olc::vi2d& cursorPos)
{
	return MenuItem::house;
}

bool Menu::isPositionInside(const olc::vi2d& cursorPos)
{
	if(
		((cursorPos.x >= pos.x) && (cursorPos.y >= pos.y))
		 &&((cursorPos.x <= (pos+size).x) && (cursorPos.y <= (pos+size).y))
	)
		return true;

	return false;
}
enum class BuildingType{house, none};

//class building
//tent
//hunter lodge
//farm
//stockpile
//counters -> population and food. 

class WorldTile {
	olc::vi2d pos;
	uint8_t tileID;
	uint8_t buildingID;
	BuildingType buildingOnTile;

	public:
	WorldTile(olc::vi2d pos, uint8_t tileID) : pos(pos), tileID(tileID), buildingID(5), buildingOnTile(BuildingType::none) {};
	WorldTile(): pos({0,0}), tileID(0), buildingID(5), buildingOnTile(BuildingType::none) {};
	uint8_t getTileID() {return tileID;}
	void setPositon(int x, int y) {pos.x = x; pos.y = y;}
	void setTileID(uint8_t tileID) { this->tileID = tileID;}
	uint8_t getBuildingID()
    {
        if (buildingOnTile != BuildingType::none)
            return buildingID;
        return 0;
    }
	void setBuildingOnTile(BuildingType buildng);
};

void WorldTile::setBuildingOnTile(BuildingType buildng)
{
	if ((money - cHousePrice >= 0) && (BuildingType::none == buildingOnTile))
	{
		buildingOnTile = buildng;
		money -= cHousePrice;
        numberOfHouses++;
	}
}

class CityBuilder : public olc::PixelGameEngine
{
    public:
        CityBuilder() : viewportOrigin({0,0}) { sAppName = "CityBuilder";}
		// Transformed view object to make world offsetting simple

	private:

		//lets store the important counters here for now:
		//uint32_t population; // 4 bln people for a city should be enough...
		//uint32_t food_stockpile; 

		std::unique_ptr<olc::Sprite> pTexturedGrassTile;
		std::unique_ptr<olc::Sprite> pDeadGrassTile;
		std::unique_ptr<olc::Sprite> pGrassTile;
		std::unique_ptr<olc::Sprite> pMenuSprite;
		std::unique_ptr<olc::Sprite> pBuildingSprite;
		std::unique_ptr<olc::Sprite> pTerrain;
	// The world map, stored as a 1D array
	   std::vector<uint8_t> vWorldMap;
	   std::array<std::array<WorldTile, cWorldSize>,cWorldSize> worldMap;
	   vi2d viewportOrigin;
	   Menu mainMenu{};
	   MenuItem selectedItem;

	
        bool OnUserCreate() override;

        bool OnUserUpdate(float fElapsedTime) override;

        void DrawTile(int x, int y, uint8_t tileConst, uint8_t buildingID);
        olc::vi2d vBlockSize = { cTileSize,cTileSize };

        void HandleViewport(float fElapsedTime);
        void HandleMouseEvents(float fElapsedTime);
        void HandleSimulation(float fElapsedTime);

};

bool CityBuilder::OnUserCreate()
{
	// Called once at the start, so create things here

	pTexturedGrassTile = std::make_unique<olc::Sprite>("./Sprites/Ground/TexturedGrass.png");
	pDeadGrassTile = std::make_unique<olc::Sprite>("./Sprites/Ground/TexturedGrass.png");
	pGrassTile = std::make_unique<olc::Sprite>("./Sprites/Ground/Grass.png");
	pMenuSprite = std::make_unique<olc::Sprite>("./Sprites/UI/Menu.png");
	pBuildingSprite = std::make_unique<olc::Sprite>("./Sprites/Buildings/Red/RedHouses.png");
	pTerrain = std::make_unique<olc::Sprite>("./Sprites/maps/green_map.png");
	money = 100;

	selectedItem = MenuItem::none;
	//16x16 pixels tile; 160x160 tiles map
	std::fstream fs;
	fs.open("map.bin", std::ios::in);
	if(fs.is_open())
	{
		uint8_t tileArray[cWorldSize*cWorldSize];
		fs.read ((char *)tileArray, sizeof(tileArray));
		for(int y = 0, i = 0; y < cWorldSize; y++)
		{
			for(int x = 0; x < cWorldSize; x++)
			{
				worldMap[y][x].setPositon(x, y);
				worldMap[y][x].setTileID(tileArray[i++]);
			}
		}
		fs.close();
	}
	else
	{
		fs.open("map.bin", std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
		uint8_t tileArray[cWorldSize*cWorldSize];
		//generate and save the map
		for(int y = 0, i = 0; y < cWorldSize; y++)
		{
			for(int x = 0; x < cWorldSize; x++)
			{
				uint8_t tile = rand() % cTileSize + 1;
				worldMap[y][x].setPositon(x, y);
				worldMap[y][x].setTileID(tile);
				tileArray[i++] = tile;
			}
		}
		fs.write((const char *)tileArray, sizeof(tileArray));
		fs.close();
	}

	DrawSprite(0,0, pTerrain.get());
	return true;			
}

    float fAccumulatedTimeMove{0.0f};
    float fAccumulatedTimeRender{0.0f};
    float fAccumulatedTimeSim{0.0f};
// update game states
// update population based on free houses
// update resources
// calculate tick
void CityBuilder::HandleViewport(float fElapsedTime)
{
    constexpr float fTargetFrameTimeMove = 1.0f / 160.0f; // Virtual FPS of 160fps
	uint32_t x{0};
	uint32_t y{0};
	fAccumulatedTimeMove += fElapsedTime;
    if (fAccumulatedTimeMove >= fTargetFrameTimeMove)
	{
		fAccumulatedTimeMove -= fTargetFrameTimeMove;
		//fElapsedTime = fTargetFrameTimeMove;


		vi2d mousePos = GetMousePos();

		if(mousePos.x == 639)
		{
			if((viewportOrigin.x + cScreenWidth) != cMapWidth)
				viewportOrigin.x++;
		}

		if(mousePos.x == 0)
		{
			if(viewportOrigin.x != 0)
				viewportOrigin.x--;
		}

		if(mousePos.y == 479)
		{
			if((viewportOrigin.y + cScreenHeight) != cMapHeight)
				viewportOrigin.y++;
		}

		if(mousePos.y == 0)
		{
			if(viewportOrigin.y != 0)
				viewportOrigin.y--;
		}

		x = viewportOrigin.x;
		y = viewportOrigin.y;

		//draw the viewable portion of the map
		DrawPartialSprite(0, 0, pTerrain.get(), x, y, cScreenWidth, cScreenHeight);
	}
}

void CityBuilder::HandleMouseEvents(float fElapsedTime)
{
    constexpr float fTargetFrameTimeRender = 1.0f / 60.0f; // Virtual FPS of 60fps
	fAccumulatedTimeRender += fElapsedTime;
	if (fAccumulatedTimeRender >= fTargetFrameTimeRender)
	{
		fAccumulatedTimeRender -= fTargetFrameTimeRender;
		//fElapsedTime = fTargetFrameTimeRender;

			// Continue as normal
//		std::string mousePosX = "Mouse pos X: " + std::to_string(GetMousePos().x);
//		std::string mousePosY = "Mouse pos Y: " + std::to_string(GetMousePos().y);
		std::string mouseClick = "Mouse clicked: false";
		std::string selectedItemStr = "Selected item: none";
		std::string moneyStr = "Money: " + std::to_string(money);
		std::string popStr = "Population: " + std::to_string(population);

		HWButton mouseButton = GetMouse(0);


		if(mouseButton.bPressed || mouseButton.bHeld)
		{
			mouseClick = "Mouse clicked: true";

			if(mainMenu.isPositionInside(GetMousePos()))
				selectedItem = mainMenu.getMenuItem(GetMousePos());
			else
			{
				if(selectedItem == MenuItem::house)
				{
					//get the clicked tile position
					int x1 = (GetMousePos().x+(viewportOrigin.x*cTileSize))/cTileSize;
					int y1 = (GetMousePos().y+(viewportOrigin.y*cTileSize))/cTileSize;

					worldMap[y1][x1].setBuildingOnTile(BuildingType::house);
					//selectedItem = MenuItem::none;
				}
			}
		}

		if(MenuItem::none == selectedItem)
			selectedItemStr = "Selected item: none";
		
		if(MenuItem::house == selectedItem)
			selectedItemStr = "Selected item: house";

//		DrawString(0, 0, mouseClick, olc::BLACK, 2);
//		DrawString(0, 15, selectedItemStr, olc::BLACK, 2);
//		DrawString(0, 30, moneyStr, olc::BLACK, 2);
//		DrawString(0, 45, popStr, olc::BLACK, 2);

		//draw the menu
//		DrawSprite(mainMenu.getOrigin(), pMenuSprite.get());
//		DrawRect(mainMenu.getOrigin(), mainMenu.getSize(), olc::BLACK);
	//	FillRect(mainMenu.getOrigin(), mainMenu.getSize(), olc::CYAN);
		//draw the cursor. The game objects will keep the state, the drawing is separate. If the cursor needs to display a house for instance, let the rendere check  and draw it.
	}
}

void CityBuilder::HandleSimulation(float fElapsedTime)
{
    constexpr float fTargetFrameSimTime = 1.0f; // update each second (1 fps)                                             
    fAccumulatedTimeSim += fElapsedTime;
	if (fAccumulatedTimeSim >= fTargetFrameSimTime)
	{
        fAccumulatedTimeSim -= fTargetFrameSimTime;
        uint32_t maxPopulation = numberOfHouses*4;
		fAccumulatedTimeSim -= fTargetFrameSimTime;
        money += population; //to tweak... some kind of taxation ration maybe 
        if(population < maxPopulation)
            population += 1; //also to tweak population growth
    }
}


bool CityBuilder::OnUserUpdate(float fElapsedTime)
{

	//check for free house
	//for one free house increase the population with four
	//if no free houses increase population with 1 per 2 citizens (pressure for houses)
    HandleViewport(fElapsedTime);
    HandleMouseEvents(fElapsedTime);
    HandleSimulation(fElapsedTime);

    HWButton mouseButtonR = GetMouse(1);
    if(mouseButtonR.bPressed)
    {
        if(MenuItem::house == selectedItem)
            selectedItem = MenuItem::none;
    }


	return true;
}

void CityBuilder::DrawTile(int x, int y, uint8_t tileConst, uint8_t buildingID)
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
	if(5 == buildingID)
	{
		DrawPartialSprite(olc::vi2d(x, y), pBuildingSprite.get(), olc::vi2d(2, 1) * vBlockSize, vBlockSize);
	}
}



int main()
{
    CityBuilder game;
    game.Construct(cScreenWidth, cScreenHeight, 2, 2);
    game.Start();

    return 0;
}
