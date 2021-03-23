#include "bmph.h"
#include<iostream>
#include<fstream>
using namespace std;
Color::Color() : r(0), g(0), b(0) {
}
Color::Color(float r, float g, float b) : r(r), g(g), b(b) {
}

Color::~Color() {
}

bmph::bmph(int w, int h) :myW(w), myH(h), myColor(vector<Color>(w* h)) {
}
bmph::~bmph() {
}
Color bmph::getColor(int a, int b) const {
	return myColor[b * myW + a];
}
void bmph::setColor(const Color& color, int a, int b) {
	myColor[b * myW + a].r = color.r;
	myColor[b * myW + a].g = color.b;
	myColor[b * myW + a].b = color.b;
}
void bmph::readFile(const char* path){
	ifstream f;
	f.open(path, ios::in | ios::binary);
	
	if (!f.is_open()) {
		cout << "������ �� �� �����ϴ�.\n";
		return;
	}
	const int fileHeaderSize = 14;
	const int informationHeaderSize = 40;

	unsigned char fileHeader[fileHeaderSize];
	f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

	if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
		cout << "bmp�� �ƴմϴ�.\n";
		f.close();
		return;
	}


	unsigned char informationHeader[informationHeaderSize];
	f.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

	int filesize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
	myW = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
	myH = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

	myColor.resize(myW * myH);

	const int paddingAmount = ((4 - (myW * 3) % 4) % 4);
	for (int i = 0; i < myH; i++) {
		for (int j = 0; j < myW; j++) {
			unsigned char color[3];
			f.read(reinterpret_cast<char*>(color), 3);

			myColor[i * myW + j].r = static_cast<float>(color[2]) / 255.0f;
			myColor[i * myW + j].g = static_cast<float>(color[1]) / 255.0f;
			myColor[i * myW + j].b = static_cast<float>(color[0]) / 255.0f;
		}
		f.ignore(paddingAmount);
	}
	f.close();
	
	cout << "������ �о����ϴ�.\n";
}
void bmph::Export(const char* path) {
	ofstream f;
	f.open(path, ios::out | ios::binary);

	if (!f.is_open()) {
		cout << "������ �� �� �����ϴ�.\n";
		return;
	}
	unsigned char bmpPadding[3] = { 0,0,0 };
	const int paddingAmount = ((4 - (myW * 3) % 4) % 4);

	const int headerSize = 14;
	const int informationheaderSise = 40;
	const int fileSize = headerSize + informationheaderSise + myW * myH * 3 + paddingAmount * myH;
	unsigned char fileHeader[headerSize];
	fileHeader[0] = 'B';
	fileHeader[1] = 'M';
	fileHeader[2] = fileSize;
	fileHeader[3] = fileSize >> 8;
	fileHeader[4] = fileSize >> 16;
	fileHeader[5] = fileSize >> 24;
	fileHeader[6] = 0;
	fileHeader[7] = 0;
	fileHeader[8] = 0;
	fileHeader[9] = 0;
	fileHeader[10] = headerSize + informationheaderSise;
	fileHeader[11] = 0;
	fileHeader[12] = 0;
	fileHeader[13] = 0;

	unsigned char informationHeader[informationheaderSise];

	informationHeader[0] = informationheaderSise;
	informationHeader[1] = 0;
	informationHeader[2] = 0;
	informationHeader[3] = 0;
	informationHeader[4] = myW;
	informationHeader[5] = myW >> 8;
	informationHeader[6] = myW >> 16;
	informationHeader[7] = myW >> 24;
	informationHeader[8] = myH;
	informationHeader[9] = myH >> 8;
	informationHeader[10] = myH >> 16;
	informationHeader[11] = myH >> 24;
	informationHeader[12] = 1;
	informationHeader[13] = 0;
	informationHeader[14] = 24;
	for (int i = 15; i < 37; i++) {
		informationHeader[i] = 0;
	}
	f.write(reinterpret_cast<char*>(fileHeader), headerSize);
	f.write(reinterpret_cast<char*>(informationHeader), informationheaderSise);

	for (int k = 0; k < myH; k++) {
		for (int a = 0; a < myW; a++) {
			unsigned char r = static_cast<unsigned char>(getColor(a, k).r * 255.0f);
			unsigned char g = static_cast<unsigned char>(getColor(a, k).g * 255.0f);
			unsigned char b = static_cast<unsigned char>(getColor(a, k).b * 255.0f);

			unsigned char color[] = { b, r, g };

			f.write(reinterpret_cast<char*>(color), 3);
		}
		f.write(reinterpret_cast<char*>(bmpPadding), paddingAmount);
	}
	f.close();
	cout << "������ ����������ϴ�.\n";
}

