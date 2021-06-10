#include "ImageProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

ImageProcessing::ImageProcessing(const char *_inImgName,
                                 const char *_outImgName,
                                 int *_height,
                                 int *_width,
                                 int *_bitDepth,
                                 unsigned char *_header,
                                 unsigned char *_colorTable,
                                 unsigned char *_inBuffer,
                                 unsigned char *_outBuffer)
{
    inImgName = _inImgName;
    outImgName = _outImgName;
    height = _height;
    width = _width;
    bitDepth = _bitDepth;
    header = _header;
    colorTable = _colorTable;
    inBuffer = _inBuffer;
    outBuffer = _outBuffer;
}

void ImageProcessing::imread()
{
    int i;
    FILE *streamIn;
    streamIn = fopen(inImgName, "rb");
    if(streamIn == (FILE *)0)
    {
        cout << "Unable to open file. Perhaps file does not exist." << endl;
        exit(0);
    }
    for(i=0; i<BMP_HEADER_SIZE; i++)
    {
        header[i] = getc(streamIn);
    }

    *width = *(int *)&header[18];
    *height = *(int *)&header[22];
    *bitDepth = *(int *)&header[28];

    if (*bitDepth <=8)
    {
        fread(colorTable, sizeof(unsigned char), BMP_COLOR_TABLE_SIZE, streamIn);
    }

    fread(inBuffer, sizeof(unsigned char), _512by512_IMG_SIZE, streamIn);
    fclose(streamIn);
}

void ImageProcessing::imwrite()
{
    FILE *fout = fopen(outImgName, "wb");
    fwrite(header, sizeof(unsigned char), BMP_HEADER_SIZE, fout);
    if(*bitDepth <= 8)
    {
        fwrite(colorTable, sizeof(unsigned char), BMP_COLOR_TABLE_SIZE, fout);
    }
    fwrite(outBuffer, sizeof(unsigned char), _512by512_IMG_SIZE, fout);
    fclose(fout);
}

void ImageProcessing::imcopy(unsigned char *_srcBuffer, unsigned char *_destBuffer, int bufferSize)
{
    for(int i = 0; i < bufferSize; i++)
    {
        _destBuffer[i] = _srcBuffer[i];
    }
}

void ImageProcessing::imbinarize(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int threshold)
{

    for (int i = 0; i < imgSize; i++)
    {
        _outImgData[i] = (_inImgData[i] > threshold) ? WHITE :BLACK;
    }
}

void ImageProcessing::imbrighten(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int factor)
{
    for (int i = 0; i < imgSize; i++)
    {
        int temp = _inImgData[i] + factor;
        _outImgData[i] = (temp > MAX_COLOR) ? MAX_COLOR :temp;
    }
}

void ImageProcessing::imdim(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int factor)
{
    for (int i = 0; i < imgSize; i++)
    {
        int temp = _inImgData[i] - factor;
        _outImgData[i] = (temp < MIN_COLOR) ? MIN_COLOR :temp;
    }
}

void ImageProcessing::imhist(unsigned char *_imgData, int imgRows, int imgCols, float hist[], const char *histFile)
{
    cout << "Rows: " << imgRows << endl;
    cout << "Cols: " << imgCols << endl;

    FILE *fptr;
    fptr = fopen(histFile, "w");

    int x, y, i, j;
    long int ihist[256], sum;

    for (i = 0; i <= 255; i++)
    {
        ihist[i] = 0;
    }
    sum = 0;

    for (y = 0; y < imgRows; y++)
    {
        for (x = 0; x < imgCols; x++)
        {
            j = *(_imgData+x+y*imgCols);
            ihist[j] = ihist[j] + 1;
            sum = sum + 1;
        }
    }

    for (i = 0; i <= 255; i++)
    {
        hist[i] = (float)ihist[i]/(float)sum;
    }

    for (i = 0; i <= 255; i++)
    {
        fprintf(fptr, "\n%f",hist[i]);
    }
    fclose(fptr);
}

void ImageProcessing::imhisteq(unsigned char *_inImgData, unsigned char *_outImgData, int imgRows, int imgCols)
{
    int x, y, i, j;
    int histeq[256];
    float hist[256];
    float sum;

    const char initHist[] = "hists/init_hist.txt";
    const char finalHist[] = "hists/final_hist.txt";

    imhist(&_inImgData[0], imgRows, imgCols, &hist[0], initHist);

    for (i = 0; i <= 255; i++)
    {
        sum = 0.0;
        for (j = 0; j <= i; j++)
        {
            sum = sum + hist[j];
        }
        histeq[i] = (int)(255*sum+0.5);
    }

    for (y = 0; y < imgRows; y++)
    {
        for (x = 0; x < imgCols; x++)
        {
            *(_outImgData+x+y*imgCols) = histeq[*(_inImgData+x+y*imgCols)];
        }
    }

    imhist(&_outImgData[0], imgRows, imgCols, &hist[0], finalHist);
}

void ImageProcessing::imnegative(unsigned char *_inImgData, unsigned char *_outImgData, int imgWidth, int imgHeight)
{
    for (int i = 0; i < imgHeight; i++)
    {
        for (int j = 0; j < imgWidth; j++)
        {
            _outImgData[i*imgWidth+j] = 255-_inImgData[i*imgWidth+j];

        }
    }
}

void ImageProcessing::imconv2d(int imgRows, int imgCols, struct Mask *myMask, unsigned char *inBuffer, unsigned char *outBuffer)
{
    long i,j,m,n,idx,jdx;
    int ms,im,val;
    unsigned char *tmp;

    for (i = 0; i < imgRows; i++)
    {
        for (j = 0; j < imgCols; j++)
        {
            val = 0;
            for (m = 0; m < myMask->Rows; ++m)
            {
                for (n = 0; n < myMask->Cols; ++n)
                {
                    ms = (signed char)*(myMask->Data + m * myMask->Rows + n);
                    idx = i - m;
                    jdx = j - n;

                    if ((idx >= 0) && (jdx >= 0))
                    {
                        im = *(inBuffer + idx + imgRows + jdx);
                    }
                    val += ms * im;
                }
            }
        }
        if (val > 255) { val = 255; }
        if (val < 0) { val = 0; }
        tmp = outBuffer + i * imgRows + j;
        *tmp = (unsigned char)val;
    }
}

void ImageProcessing::imlinedetect(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows, const int MASK[][3])
{
    int x, y, i, j, sum;
    for (y = 1; y <= imgRows; y++)
    {
        for (x = 1; x <= imgCols; x++)
        {
            sum = 0;
            for (i = -1; i <= 1; i++)
            {
                for (j = -1; j <= 1; j++)
                {
                    sum = sum + *(_inImgData + x + i + (long)(y + j) * imgCols) * MASK[i+1][j+1];
                }
            }
            if (sum > 255){ sum = 255; }
            if (sum < 0 ){ sum = 0; }

            *(_outImgData + x + (long)y * imgCols) = sum;
        }
    }
}

void ImageProcessing::setmask(int mskRows, int mskCols, const int mskData[])
{
    signed char *tmp;
    int reqSize;

    myMask.Rows = mskRows;
    myMask.Cols = mskCols;
    reqSize = myMask.Rows * myMask.Cols;

    myMask.Data = (unsigned char *)malloc(reqSize);

    tmp = (signed char *)myMask.Data;

    for (int i = 0; i < reqSize; i++)
    {
        *tmp = mskData[i];
        ++tmp;
    }
}

void ImageProcessing::guassnoise(unsigned char *_inImgData, int imgCols, int imgRows, float var, float mean)
{
    int x, y;
    float theta, noise;

    for (y = 0; y < imgRows; y++)
    {
        for (x = 0; x < imgCols; x++)
        {
            noise = sqrt(-2 * var * log(1.0-(float)rand()/32767.1));
            theta = (float)rand() * 1.9175345e-4 - 3.14159265;
            noise = noise * cos(theta);
            noise += mean;
            if (noise > 255) {noise = 255;}
            if (noise < 0) {noise = 0;}

            *(_inImgData + x + (long)y * imgCols) = (unsigned char)(noise + 0.5);
        }
    }
}

void ImageProcessing::saltandpepper(unsigned char *_inImgData, int imgCols, int imgRows, float prob)
{
    int x, y, data1, data2, data;
    data = (int)(prob * 32768/2);
    data1 = data + 16384;
    data2 = 16384 - data;

    for (y = 0; y < imgRows; y++)
    {
        for (x = 0; x < imgCols; x++)
        {
            data = rand();
            if (data >= 16384 && data < data1)
            {
                *(_inImgData + x + (long)y * imgCols) = 0;
            }
            else if (data >= data2 && data < 16384)
            {
                *(_inImgData + x + (long)y * imgCols) = 255;
            }
        }
    }
}

void ImageProcessing::maxfilter(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows)
{
    int x, y, i, j, smax;
    int a[11][11];
    int n = 3;

    for (y = n/2; y < imgCols-n/2; y++)
    {
        for (x = n/2; x < imgRows-n/2; x++)
        {
            smax = 0;
            for (j =- n/2; j <= n/2; j++)
            {
                for (i -= n/2; i <= n/2; i++)
                {
                    a[i+n/2][j+n/2] = *(_inImgData + x + i + (long)(y + j) * imgCols);
                }
            }
            for (j = 0; j <= n-1; j++)
            {
                for (i = 0; i < n-1; i++)
                {
                    if (a[i][j] > smax) {smax = a[i][j];}
                }
            }
            *(_outImgData + x + (long)y * imgCols) = smax;
        }
    }
}

void ImageProcessing::medianfilter(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows)
{
    int x, y, i, j, z;
    int n, ar[121], a;
    n = 7;

    for (y = n/2; y < imgRows - n/2; y++)
    {
        for (x = n/2; x < imgCols - n/2; x++)
        {
            z = 0;
            for (j =- n/2; j <= n/2; j++)
            {
                for (i =- n/2; i <= n/2; i++)
                {
                    ar[z] = *(_inImgData + x + i + (long)(y + j) * imgCols);
                    z++;
                }
            }
            for (j = 1; j <= n * n-1; j++)
            {
                a = ar[j];
                i = j - 1;
                while (i >= 0 && ar[i] > a)
                {
                    ar[i+1] = ar[i];
                    i--;
                }
                ar[i+1] = a;
            }
            *(_outImgData + x + (long)y * imgCols) = ar[n*n/2];
        }
    }
}

void ImageProcessing::minfilter(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows)
{
    int x, y, i, j, smin, n, a[11][11];
    n = 5;

    for (y = n/2; y < imgRows - n/2; y++)
    {
        for (x = n/2; x < imgCols - n/2; x++)
        {
            smin = 255;
            for (j =- n/2; j <= n/2; j++)
            {
                for (i =- n/2; i <= n/2; i++)
                {
                    a[i+n/2][j+n/2] = *(_inImgData + x + i + (long)(y + j) * imgCols);
                }
            }
            for (j = 0; j <= n - 1; j++)
            {
                for (i = 0; i <= n; i++)
                {
                    if (a[i][j] < smin) { smin = a[i][j]; }
                }
            }
            *(_outImgData + x + (long)y * imgCols) = smin;
        }
    }
}

ImageProcessing::~ImageProcessing()
{
    //dtor
}
