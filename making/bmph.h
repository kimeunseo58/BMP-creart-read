#pragma once

#include<vector>

struct Color {
	float r, g, b;

	Color();
	Color(float r, float g, float b);
	~Color();
};
class bmph {
public:
	bmph(int w, int h);
	~bmph();

	Color getColor(int a, int b) const;
	void setColor(const Color& color, int a, int b);

	void Export(const char* path);

private:
	int myW;
	int myH;
	std::vector<Color> myColor;
};