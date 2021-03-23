#include <iostream>
#include "bmph.h"
using namespace std;

int main() {
	const int width = 630;
	const int height = 480;

	bmph mybmp(width, height);

	for (int b = 0; b < height; b++) {
		for (int a = 0; a < width; a++) {
			mybmp.setColor(Color((float)a / (float)width, 1.0f - ((float)a / (float)width), (float)b / (float)height), a, b);

		}
	}
	mybmp.Export("mybmp.bmp");

	bmph copy(0, 0);
	copy.readFile("mybmp.bmp");
	copy.Export("copy.bmp");
}