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
    fread(&bf, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);// ���14����Ʈ
    fread(&bi, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);// ������ ���� 40����Ʈ
    unsigned char* data = (unsigned char*)malloc(bi.biSizeImage);
    fread(data, sizeof(unsigned char), bi.biSizeImage, fp);  //��Ʈ�����κ��� ������ �д´�
    bmpSize = bi.biSizeImage;
    for (int i = 0; i < bi.biSizeImage; i++) {
        myData += (bitset<8>((int)data[i])).to_string();  //���ڿ��� �����Ͱ��� 8�ڸ� �������� ��ȯ�Ͽ� �ִ´�.
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
    int size = message.length();  //���ڿ��� ���̸� ����� �޴´�.
    int* arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = message[i]; //�Է¹��� �޽��� �ѱ��ھ����� �ƽ�Ű���� �޾� ��Ʈ ��̿� �������ش�.
    }
    string* askiArr = new string[size * 7]; //�ƽ�Ű���� �������� �ٲ۰��� �������� �迭
    for (int i = 0; i < size * 7; i++) {
        askiArr[i] = bitset<7>(arr[i]).to_string(); //�ƽ�Ű���� �������� �ٲپ� ����. ������ �ƽ�Ű���� �ִ� 127�����̹Ƿ�, 2�����δ� 7�ڸ��̴�.
    }
    //bmp���� �迭�� lsb�ڸ��� string�� ��Ʈ���� �����Ѵ�.
    int k = 7;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 7; j++) {
               myData[k] = askiArr[i][j];  //myData�� ���ڿ��ε�, LSB�� 8�ڸ����� �����Ƿ�, 8������ ��� ��ġ���ش�.
               k += 8;
        }
    }

}

int make2To10(string a) {
    int numResult = 0; //��Ʈ�� �������� �������� �ٲپ��ִ� �Լ�
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
    //addMessage�� ���� ������� �迭�� bmp���Ͽ� �����Ѵ�.
    //�޽����� �⺻ ������ �����ִ� myData�� �ٽ� unsigned char������ ������ش�.
    string k;
    unsigned char* newDataM = new unsigned char[myData.length()]; //������ ���� unsigned char �迭
    int h = 0;
    for (int i = 0; i < myData.length(); i += 8) {
        k = myData.substr(i, 8); //�迭�� 8���� �����ش�.
        unsigned char b = (unsigned char)make2To10(k); //����ȯ
        newDataM[h] = b;
        h++;
    }
    return newDataM;
}

string extractMessage() {
    //origin.bmp��  stego.bmp�� ���Ͽ�, ���� �ٸ��������� ����Ѵ�.
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
    //������ �޽������� LSB�� �̾Ƽ� �����Ѵ�.
    int counter = 0;
    for (int i = 7; i < bmpSize; i += 8) {
        extractOfOrigin[counter] = messageOfOrigin[i];
        extractOfStego[counter] = messageOfStego[i];
        counter++;
    }
    string k;
    int h = 0;
    messageOfOrigin = "";
    //������ 7���� ���������� �ٽ� �ƽ�Ű��, 10������ �ٲپ��ش�
    for (int i = 0; i < extractOfOrigin.length(); i += 8) {
        k = extractOfOrigin.substr(i, 8); //�迭�� 8���� �����ش�.
        messageOfOrigin[h] = (char)make2To10(k); //����ȯ
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
        if (messageOfOrigin[i] == messageOfStego[i]) break;  //���� �ٸ��� -> ���װ���׷��Ǹ� ���ؼ� �Է¹޾� ���� �κ��̹Ƿ�
        finalString[i] = messageOfStego[i];                  //���� �ٸ� �κи� �����Ѵ�.   
    }
    return finalString;

}

int main(int argc, char* argv[]) {
    char x;
    cin >> x;
    cin.ignore();
    if (x == 'e' || x == 'E') {
        //���ڵ�
        string message;   
        getline(cin, message); //������� �ޱ� ���� getline ���
        readBMP("origin.bmp");
        addMessage(message);
        writeBMP(addBPM());
    }
    else if (x == 'd' || x == 'D') {
        string mine = extractMessage();
       cout << mine;
    }
}



