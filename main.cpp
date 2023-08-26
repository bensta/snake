#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <queue>

class Snake : public olc::PixelGameEngine
{
	public: 
		Snake(){
			sAppName = "Snake";
		}
	private:
		const int nBlocksX = 20;
		const int nBlocksY = 20;
		const int nSnakeInitBlocks = 6;
		const int iSnakeInitPosX = (nBlocksX - nSnakeInitBlocks) / 2;
		const int iSnakeInitPosY = nBlocksY / 2;

		olc::vi2d vSnakeDir = { 1, 0 };
		olc::vi2d vNextSnakeDir = { 1, 0 };

		olc::vi2d vBlockSize = {16, 16};
		std::unique_ptr<int[]> blocks;
		std::unique_ptr<olc::Sprite> sprTile;

		// stores the coordinates of the snake blocks
		std::deque<olc::vi2d> snake;

		// keeps track of the food
		olc::vi2d vFoodPosition = { 3, 8 };

		bool hasColided = false;
		// time accumulator to keep track of steps
		float fAccumulatedTime = 0.0f;
		float fTimeStepLength = 0.5f;

		// Highscore
		int score = 0;

	public: 
		bool OnUserCreate() override{

			// Playing field
			blocks = std::make_unique<int[]>(nBlocksX * nBlocksY);
			for (int y = 0; y < nBlocksY; y++){
				for (int x = 0; x < nBlocksX; x++){
					if (x == 0 || y == 0 || x == nBlocksX - 1 || y == nBlocksY - 1 )
						blocks[y * nBlocksX + x] = 10;
					else
						blocks[y * nBlocksX + x] = 0;
				}
			}

			// Initialize the snake
			for (int x = 0; x < nSnakeInitBlocks; x++){
				snake.push_front(olc::vi2d(iSnakeInitPosX + x, iSnakeInitPosY));
				blocks[iSnakeInitPosY * nBlocksX + iSnakeInitPosX + x] = 1;			// Set value of snake tiles
			}

			// load the sprite
			sprTile = std::make_unique<olc::Sprite>("/home/benjamin/git/snake/gfx/tut_tiles.png");

			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override{
			fAccumulatedTime += fElapsedTime;

			// handle user input ==> handling each frame to avoid missing any input
			// going up or down and left is requested
			if (GetKey(olc::Key::LEFT).bHeld && vSnakeDir.y != 0 ) { vNextSnakeDir.x = -1; vNextSnakeDir.y = 0; }
			// going up or down and right is requested
			if (GetKey(olc::Key::RIGHT).bHeld && vSnakeDir.y != 0 ) { vNextSnakeDir.x = 1; vNextSnakeDir.y = 0; }
			// going left or right and requesting up
			if (GetKey(olc::Key::UP).bHeld && vSnakeDir.x != 0 ) { vNextSnakeDir.x = 0; vNextSnakeDir.y = -1; }
			// going left or right and requesting down
			if (GetKey(olc::Key::DOWN).bHeld && vSnakeDir.x != 0 ) { vNextSnakeDir.x = 0; vNextSnakeDir.y = 1; }
			
			if (fAccumulatedTime < fTimeStepLength || hasColided)
				return true;
			
			fAccumulatedTime = 0;
			vSnakeDir = vNextSnakeDir;
			
			// get next position of snake head
			olc::vi2d vNextHeadPos = snake.front() + vSnakeDir;

			// detect colision with wall
			hasColided |= ( vNextHeadPos.x == 0 
							|| vNextHeadPos.x == nBlocksX -1 
							|| vNextHeadPos.y == 0
							|| vNextHeadPos.y == nBlocksY -1
						);

			auto TestSnakeColision = [&](olc::vf2d point){
				for (auto it = snake.begin(); it != snake.end(); it++){
					if(*it == point){
						return true;
					}
				}
				return false;
			};

			// test if snake has colided with itself
			hasColided |= TestSnakeColision(vNextHeadPos);

			// we have colided. Stopping updating the screen
			if (hasColided) 
				return true;

			// Detect food
			bool hasEaten = false;
			if (vNextHeadPos == vFoodPosition) hasEaten = true;

			// update snake
			blocks[vNextHeadPos.y * nBlocksX + vNextHeadPos.x] = 1;
			snake.push_front(vNextHeadPos);

			// remove last snake block if snake has not eaten
			if (!hasEaten){
				olc::vi2d tail = snake.back();
				blocks[tail.y * nBlocksX + tail.x] = 0;
				snake.pop_back();
			} else{ // reset food block and choose next food block
				blocks[vFoodPosition.y * nBlocksX + vFoodPosition.x] = 1;
				do{
					vFoodPosition.x = int((float(rand())/RAND_MAX) * (nBlocksX - 2) + 1);
					vFoodPosition.y = int((float(rand())/RAND_MAX) * (nBlocksY - 2) + 1);
				} while(TestSnakeColision(vFoodPosition));

				// speedup the game
				fTimeStepLength *= 0.9;
				score += 100/fTimeStepLength;
			}

			blocks[vFoodPosition.y * nBlocksX + vFoodPosition.x] = 2;
			
			// Erase previous frame
			Clear(olc::DARK_BLUE);
			SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency

			// Draw tiles
			for (int y = 0; y < nBlocksY; y++){
				for (int x = 0; x < nBlocksX; x++){
					switch (blocks[y * nBlocksX + x]){
						case 0:
							break;
						case 10: // grey tile (border)
							DrawPartialSprite(olc::vi2d(x,y) * vBlockSize, sprTile.get(),  olc::vi2d(0, 0) * vBlockSize, vBlockSize);
							break;
						case 1: // red tile
							DrawPartialSprite(olc::vi2d(x,y) * vBlockSize, sprTile.get(),  olc::vi2d(1, 0) * vBlockSize, vBlockSize);
							break;
						case 2: // green tile
							DrawPartialSprite(olc::vi2d(x,y) * vBlockSize, sprTile.get(),  olc::vi2d(2, 0) * vBlockSize, vBlockSize);
							break;
						case 3: // yellow tile
							DrawPartialSprite(olc::vi2d(x,y) * vBlockSize, sprTile.get(),  olc::vi2d(3, 0) * vBlockSize, vBlockSize);
							break;
					}
				}
			}
			DrawString(olc::vi2d((nBlocksX + 2) * vBlockSize.x, 2 * vBlockSize.y), std::to_string(score), olc::WHITE, 2u);
			SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
			return true;
		}
};

int main(){
	Snake demo;
	if (demo.Construct(1024, 960, 2, 2))
		demo.Start();
	return 0;
}