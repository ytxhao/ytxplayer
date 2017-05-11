//
// Created by Administrator on 2017/4/11.
//

#ifndef MEDIACODECTEST_HEADERBMP_H
#define MEDIACODECTEST_HEADERBMP_H
//14byte文件头
typedef struct
{
    char cfType[2];//文件类型，"BM"(0x4D42)
    long cfSize;//文件大小（字节）
    long cfReserved;//保留，值为0
    long cfoffBits;//数据区相对于文件头的偏移量（字节）
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐

//40byte信息头
typedef struct
{
    char ciSize[4];//BITMAPFILEHEADER所占的字节数
    long ciWidth;//宽度
    long ciHeight;//高度
    char ciPlanes[2];//目标设备的位平面数，值为1
    int ciBitCount;//每个像素的位数
    char ciCompress[4];//压缩说明
    char ciSizeImage[4];//用字节表示的图像大小，该数据必须是4的倍数
    char ciXPelsPerMeter[4];//目标设备的水平像素数/米
    char ciYPelsPerMeter[4];//目标设备的垂直像素数/米
    char ciClrUsed[4]; //位图使用调色板的颜色数
    char ciClrImportant[4]; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
}__attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
    unsigned short blue;
    unsigned short green;
    unsigned short red;
    unsigned short reserved;
}__attribute__((packed)) PIXEL;//颜色模式RGB

#endif //MEDIACODECTEST_HEADERBMP_H
