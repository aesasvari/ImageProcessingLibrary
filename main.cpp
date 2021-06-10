#include <iostream>
#include "ImageProcessing.h"

using namespace std;

int main()
{
    float imgHist[NUMBER_OF_GRAYLEVELS];
    int imgWidth, imgHeight, imgBitDepth;
    unsigned char imgHeader[BMP_HEADER_SIZE];
    unsigned char imgColorTable[BMP_COLOR_TABLE_SIZE];
    unsigned char imgInBuffer[_512by512_IMG_SIZE];
    unsigned char imgOutBuffer[_512by512_IMG_SIZE];

    const char imgName[] = "images/lenaGreyscale.bmp";
    const char outName[] = "images/lenaGreyscale_prewitt.bmp";

    ImageProcessing *myImage = new ImageProcessing(
                                                   imgName,
                                                   outName,
                                                   &imgHeight,
                                                   &imgWidth,
                                                   &imgBitDepth,
                                                   &imgHeader[0],
                                                   &imgColorTable[0],
                                                   &imgInBuffer[0],
                                                   &imgOutBuffer[0]
                                                   );

    myImage->imread();
    myImage->setmask(3, 3, PREWITT_VER);
    myImage->imconv2d(imgHeight, imgWidth, &myImage->myMask, imgInBuffer, imgOutBuffer);
    myImage->imwrite();


    cout<<"Success!"<<endl;
    cout<<"Image Height: "<<imgHeight<<endl;
    cout<<"Image Width: "<<imgWidth<<endl;

    return 0;
}
