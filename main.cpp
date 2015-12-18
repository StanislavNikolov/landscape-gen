#include <png++/png.hpp>
#include <random>
#include <cstring>

const long long worldSize = (1 << 10) + 1;
int maxDiamondStepError, maxSquareStepError;

int world[worldSize][worldSize];

std::random_device randDev;
std::uniform_int_distribution<int> dist(0, 255);
std::mt19937 eng(randDev());

int rnd(int n)
{
	return (dist(eng) % n) - (dist(eng) % n);
}

void generate(unsigned x, unsigned y, unsigned sx, unsigned sy)
{
	/* Corners:
	 * 0###1
	 * #####
	 * #####
	 * 2###3
	 */
	int c[4];
	c[0] = world[x][y];
	c[1] = world[x+sx][y];
	c[2] = world[x][y+sy];
	c[3] = world[x+sx][y+sy];

	if(world[x+sx/2][y+sy/2] == 0)
		world[x+sx/2][y+sy/2] = (c[0] + c[1] + c[2] + c[3]) / 4 + rnd(maxDiamondStepError);

	int cen = world[x+sx/2][y+sy/2];

	if(world[x+sx/2][y] == 0)
		world[x+sx/2][y] = (cen + c[0] + c[1]) / 3 + rnd(maxSquareStepError);

	if(world[x][y+sy/2] == 0)
		world[x][y+sy/2] = (cen + c[0] + c[2]) / 3 + rnd(maxSquareStepError);

	if(world[x+sx][y+sy/2] == 0)
		world[x+sx][y+sy/2] = (cen + c[1] + c[3]) / 3 + rnd(maxSquareStepError);

	if(world[x+sx/2][y+sy] == 0)
		world[x+sx/2][y+sy] = (cen + c[2] + c[3]) / 3 + rnd(maxSquareStepError);

	if(sx >= 4)
	{
		generate(x, y, sx/2, sy/2);
		generate(x+sx/2, y, sx/2, sy/2);
		generate(x, y+sy/2, sx/2, sy/2);
		generate(x+sx/2, y+sy/2, sx/2, sy/2);
	}
}

int main()
{
	std::cout << "Max diamond step: ";
	std::cin >> maxDiamondStepError;
	std::cout << "Max square step: ";
	std::cin >> maxSquareStepError;

	png::image< png::rgb_pixel > image(worldSize, worldSize);

	world[0][0] 					= dist(eng);
	world[worldSize-1][0] 			= dist(eng);
	world[0][worldSize-1] 			= dist(eng);
	world[worldSize-1][worldSize-1] = dist(eng);
	world[(worldSize-1)/2][(worldSize-1)/2] = 200;

	std::cout << "Generating the image..." << std::endl;

	generate(0, 0, worldSize-1, worldSize-1);

	//               water/ocean|sand|grass|forest|hills|snow
	int layerLevel[] = {30, 60, 90, 100, 140, 180, 220, 256};
	int layer[][3] = {	{0, 0, 100}, 		// 30 very dark blue
						{0, 0, 150}, 		// 60 dark blue
						{0, 0, 200}, 		// 90 blue
						{237, 201, 175},	// 100 sand
						{44, 176, 55}, 		// 120 grass
						{0, 90, 0}, 		// 160 forest
						{0, 40, 0}, 		// 220 hills
						{255, 255, 255}, 	// 256 snow
	};

	for(unsigned y = 1;y < worldSize-1;++ y)
	{
		for(unsigned x = 1;x < worldSize-1;++ x)
		{
			int v = world[y-1][x] + world[y+1][x]
						+ world[y][x-1] + world[y][x+1]
						+ world[y-1][x-1] + world[y-1][x+1]
						+ world[y+1][x-1] + world[y+1][x+1]
						+ world[y][x];
			world[y][x] = v / 9;
		}
	}

	// Converts the height map to an png::image
	for(png::uint_32 y = 0;y < worldSize;++ y)
	{
		for(png::uint_32 x = 0;x < worldSize;++ x)
		{
			for(unsigned i = 0;;i ++)
			{
				if(world[y][x] < layerLevel[i] or world[y][x] >= 256)
				{
					image[y][x] = png::rgb_pixel(layer[i][0], layer[i][1], layer[i][2]);
					break;
				}
			}
		}
	}

	std::cout << "Saving the image..." << std::endl;
	image.write("output2.png");
	std::cout << "Done!" << std::endl;

	return 0;
}
