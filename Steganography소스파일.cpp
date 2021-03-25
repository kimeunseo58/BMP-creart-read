#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>
#include<stdio.h>
#include<bitset>
#include<vector>
#include <sstream>
#include <iomanip>


using namespace std;

BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
ifstream fin;
FILE* fp;
string myData;
int bmpSize;

unsigned char* readBMP(const char* filename)
{
    fin.open("origin.bmp", ios_base::in | ios_base::binary);
    fin.read((char*)&bf, sizeof(BITMAPFILEHEADER)); 
    fp = fopen("origin.bmp", "rb");
    fread(&bf, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);// 헤더14바이트
    fread(&bi, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);// 나머지 정보 40바이트
    unsigned char* data = (unsigned char*)malloc(bi.biSizeImage);
    fread(data, sizeof(unsigned char), bi.biSizeImage, fp);  //비트맵으로부터 정보를 읽는다
    bmpSize = bi.biSizeImage;
    for (int i = 0; i < bi.biSizeImage; i++) {
        myData += (bitset<8>((int)data[i])).to_string();  //문자열에 데이터값을 8자리 이진수로 변환하여 넣는다.
    }

    return data;
}

void writeBMP(unsigned char* data) {
    fp = fopen("stego.bmp", "wb"); 
    fwrite(&bf, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&bi, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);
    fwrite(data, sizeof(unsigned char), bi.biSizeImage, fp);
    fclose(fp);

    free(data);
}

void addMessage(string message) {
    int size = message.length();  //문자열의 길이를 사이즈에 받는다.
    int* arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = message[i]; //입력받은 메시지 한글자씩마다 아스키값을 받아 인트 어레이에 저장해준다.
    }
    string* askiArr = new string[size * 7]; //아스키값을 이진수로 바꾼것을 저장해줄 배열
    for (int i = 0; i < size * 7; i++) {
        askiArr[i] = bitset<7>(arr[i]).to_string(); //아스키값을 이진수로 바꾸어 저장. 문자의 아스키값은 최대 127까지이므로, 2진수로는 7자리이다.
    }
    //bmp정보 배열의 lsb자리에 string의 비트들을 저장한다.
    int k = 7;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 7; j++) {
               myData[k] = askiArr[i][j];  //myData는 문자열인데, LSB는 8자리마다 있으므로, 8개마다 끊어서 배치해준다.
               k += 8;
        }
    }

}

int make2To10(string a) {
    int numResult = 0; //스트링 이진수룰 십진수로 바꾸어주는 함수
    int num = 1;
    for (int i = a.length() - 1; i >= 0; i--) {
        for (int j = 0; j < a.length() - i - 1;j++) {
            num *= 2;
        }
        if (a[i] == '1') numResult += num;

        num = 1;
    }
    return numResult;
}

unsigned char* addBPM() {
    //addMessage를 통해 만들어진 배열을 bmp파일에 저장한다.
    //메시지와 기본 정보가 섞여있는 myData를 다시 unsigned char형으로 만들어준다.
    string k;
    unsigned char* newDataM = new unsigned char[myData.length()]; //정보를 받을 unsigned char 배열
    int h = 0;
    for (int i = 0; i < myData.length(); i += 8) {
        k = myData.substr(i, 8); //배열을 8개씩 끊어준다.
        unsigned char b = (unsigned char)make2To10(k); //형변환
        newDataM[h] = b;
        h++;
    }
    return newDataM;
}

string extractMessage() {
    //origin.bmp와  stego.bmp를 비교하여, 서로 다른곳까지만 계산한다.
    unsigned char* originData = readBMP("origin.bmp");
    unsigned char* stegoData = readBMP("stego.bmp");
    string messageOfOrigin;
    string messageOfStego;
    for (int i = 0; i <= bmpSize; i++) {
        messageOfOrigin += (bitset<8>((int)originData[i])).to_string();
        messageOfStego += (bitset<8>((int)stegoData[i])).to_string();
    }
    string extractOfOrigin;
    string extractOfStego;
    //생성된 메시지에서 LSB만 뽑아서 추출한다.
    int counter = 0;
    for (int i = 7; i < bmpSize; i += 8) {
        extractOfOrigin[counter] = messageOfOrigin[i];
        extractOfStego[counter] = messageOfStego[i];
        counter++;
    }
    string k;
    int h = 0;
    messageOfOrigin = "";
    //끊어준 7개의 이진수들을 다시 아스키값, 10진수로 바꾸어준다
    for (int i = 0; i < extractOfOrigin.length(); i += 8) {
        k = extractOfOrigin.substr(i, 8); //배열을 8개씩 끊어준다.
        messageOfOrigin[h] = (char)make2To10(k); //형변환
        h++;
    }
    messageOfStego = "";
    h = 0;
    for (int i = 0; i < extractOfStego.length(); i += 8) {
        k = extractOfStego.substr(i, 8);
        messageOfStego[h] = (char)make2To10(k);
        h++;
    }
    string finalString;
    for (int i = 0; i < messageOfStego.length(); i++) {
        if (messageOfOrigin[i] == messageOfStego[i]) break;  //서로 다르다 -> 스테가노그래피를 통해서 입력받아 변한 부분이므로
        finalString[i] = messageOfStego[i];                  //서로 다른 부분만 추출한다.   
    }
    return finalString;

}

int main(int argc, char* argv[]) {
    char x;
    cin >> x;
    cin.ignore();
    if (x == 'e' || x == 'E') {
        //인코딩
        string message;   
        getline(cin, message); //띄어쓰기까지 받기 위해 getline 사용
        readBMP("origin.bmp");
        addMessage(message);
        writeBMP(addBPM());
    }
    else if (x == 'd' || x == 'D') {
        string mine = extractMessage();
       cout << mine;
    }
}



