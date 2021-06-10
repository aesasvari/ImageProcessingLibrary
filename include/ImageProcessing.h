#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

static const int _512by512_IMG_SIZE = 262144;
static const int BMP_COLOR_TABLE_SIZE = 1024;
static const int BMP_HEADER_SIZE = 54;
static const int MIN_COLOR = 0;
static const int MAX_COLOR = 255;
static const int BLACK = MIN_COLOR;
static const int WHITE = MAX_COLOR;
static const int NUMBER_OF_GRAYLEVELS = 255;

struct Mask{
    int Rows;
    int Cols;
    unsigned char *Data;
};

static const int LINE_DETECTOR_HOR_MSK[3][3] = {{-1,2,-1},
                                                {-1,2,-1},
                                                {-1,2,-1}
                                                };

static const int LINE_DETECTOR_VER_MSK[3][3] = {{-1,-1,-1},
                                                {2, 2, 2},
                                                {-1,-1,-1}
                                                };

static const int LINE_DETECTOR_LDIA_MSK[3][3] = {{2,-1,-1},
                                                 {-1,2,-1},
                                                 {-1,-1,2}
                                                 };

static const int LINE_DETECTOR_RDIA_MSK[3][3] = {{-1,-1,2},
                                                 {-1,2,-1},
                                                 {2,-1,-1}
                                                 };

static const int PREWITT_VER[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
static const int PREWITT_HOR[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

static const int SOBEL_VER[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
static const int SOBEL_HOR[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

static const int ROBINSON_N[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
static const int ROBINSON_NW[] = {0, 1, 2, -1, 0, 1, -2, -1, 0};
static const int ROBINSON_W[] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
static const int ROBINSON_SW[] = {2, 1, 0, 1, 0, -1, 0, -1, -2};
static const int ROBINSON_S[] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
static const int ROBINSON_SE[] = {0, -1, -2, 1, 0, -1, 2, 1, 0};
static const int ROBINSON_E[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
static const int ROBINSON_NE[] = {-2, -1, 0, -1, 0, 1, 0, 1, 2};

static const int ROBERTS_GX[] = {1, 0, 0, -1};
static const int ROBERTS_GY[] = {0, 1, -1, 0};

static const int KIRSCH_N[] = {5, 5, 5, -3, 0, -3, -3, -3, -3};
static const int KIRSCH_NW[] = {5, 5, -3, 5, 0, -3, -3, -3, -3};
static const int KIRSCH_W[] = {5, -3, -3, 5, 0, -3, 5, -3, -3};
static const int KIRSCH_SW[] = {-3, -3, -3, 5, 0, -3, 5, 5, -3};
static const int KIRSCH_S[] = {-3, -3, -3, -3, 0, -3, 5, 5, 5};
static const int KIRSCH_SE[] = {-3, -3, -3, -3, 0, 5, -3, 5, 5};
static const int KIRSCH_E[] = {-3, -3, 5, -3, 0, 5, -3, -3, 5};
static const int KIRSCH_NE[] = {-3, 5, 5, -3, 0, 5, -3, -3, -3};

static const int LAPLACIAN_POS[] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
static const int LAPLACIAN_NEG[] = {0, -1, 0, -1, 4, -1, 0, -1, 0};

static const int HP_3X3[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};

class ImageProcessing
{
    public:
        ImageProcessing(const char *_inImgName,
                                 const char *_outImgName,
                                 int *_height,
                                 int *_width,
                                 int *_bitDepth,
                                 unsigned char *_header,
                                 unsigned char *_colorTable,
                                 unsigned char *_inBuffer,
                                 unsigned char *_outBuffer);

        Mask myMask;

        void imread();
        void imwrite();
        void imcopy(unsigned char *_srcBuffer, unsigned char *_destBuffer, int bufferSize);
        void imbinarize(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int threshold);
        void imbrighten(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int factor);
        void imdim(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int factor);
        void imhist(unsigned char *_imgData, int imgRows, int imgCols, float hist[], const char *histFile);
        void imhisteq(unsigned char *_inImgData, unsigned char *_outImgData, int imgRows, int imgCols);
        void imnegative(unsigned char *_inImgData, unsigned char *_outImgData, int imgWidth, int imgHeight);
        void imconv2d(int imgRows, int imgCols, struct Mask *myMask, unsigned char *inBuffer, unsigned char *outBuffer);
        void imlinedetect(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows, const int MASK[][3]);
        void setmask(int mskRows, int mskCols, const int mskData[]);
        void saltandpepper(unsigned char *_inImgData, int imgCols, int imgRows, float prob);
        void guassnoise(unsigned char *_inImgData, int imgCols, int imgRows, float var, float mean);
        void maxfilter(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows);
        void medianfilter(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows);
        void minfilter(unsigned char *_inImgData, unsigned char *_outImgData, int imgCols, int imgRows);

        virtual ~ImageProcessing();

    protected:

    private:
        const char *inImgName;
        const char *outImgName;
        int *height;
        int *width;
        int *bitDepth;
        unsigned char *header;
        unsigned char *colorTable;
        unsigned char *inBuffer;
        unsigned char *outBuffer;
};

#endif // IMAGEPROCESSING_H
