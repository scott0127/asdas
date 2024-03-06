#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef long INT32;
typedef unsigned short int INT16;
typedef unsigned char U_CHAR;

#define UCH(x) ((int)(x))
#define GET_2B(array, offset) ((INT16)UCH(array[offset]) + \
                               (((INT16)UCH(array[offset + 1])) << 8))
#define GET_4B(array, offset) ((INT32)UCH(array[offset]) +               \
                               (((INT32)UCH(array[offset + 1])) << 8) +  \
                               (((INT32)UCH(array[offset + 2])) << 16) + \
                               (((INT32)UCH(array[offset + 3])) << 24))
#define FREAD(file, buf, sizeofbuf) \
   ((size_t)fread((void *)(buf), (size_t)1, (size_t)(sizeofbuf), (file)))

int ReadDataSize(char *name);
void ReadImageData(char *name, U_CHAR *bmpfileheader, U_CHAR *bmpinfoheader, U_CHAR *color_table, U_CHAR *data);

#define centerX 500
#define centerY 700
#define radius 50
#define sideLength 400
int isInsideTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3)
{
   int area = abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2);
   int area1 = abs((x * (y2 - y3) + x2 * (y3 - y) + x3 * (y - y2)) / 2);
   int area2 = abs((x1 * (y - y3) + x * (y3 - y1) + x3 * (y1 - y)) / 2);
   int area3 = abs((x1 * (y2 - y) + x2 * (y - y1) + x * (y1 - y2)) / 2);

   if (area == area1 + area2 + area3)
   {
      return 1;
   }
   return 0;
}
int main()
{
   FILE *output_file = 0;

   U_CHAR bmpfileheader1[14] = {0};
   U_CHAR bmpinfoheader1[40] = {0};
   U_CHAR *data1, *data2, *new_data, color_table1[1024];

   INT32 biWidth = 0;
   INT32 biHeight = 0;

   int i, j, k, temp, biWidth4, sum;
   int histo_table[256] = {0};

   i = ReadDataSize("Fig2.20.bmp");
   data1 = (U_CHAR *)malloc(i);
   if (data1 == NULL)
   {
      exit(0);
   }

   ReadImageData("Fig2.20.bmp", bmpfileheader1, bmpinfoheader1, color_table1, data1);
   biWidth = GET_4B(bmpinfoheader1, 4);
   biHeight = GET_4B(bmpinfoheader1, 8);

   //
   // i = ReadDataSize("new2.bmp");
   data2 = (U_CHAR *)malloc(i);
   if (data2 == NULL)
   {
      exit(0);
   }

   // Process the file
   biWidth4 = ((biWidth * 1 + 3) / 4 * 4);
   temp = (radius) * (radius);
   //
   for (i = 0; i < biHeight; i++)
   {
      int height = sideLength * sqrt(3) / 2;
      int x1 = centerX, y1 = centerY;
      int x2 = centerX + sideLength / 2, y2 = centerY - height;
      int x3 = centerX - sideLength / 2, y3 = centerY - height;

      for (i = 0; i < biHeight; i++)
      {
         k = i * biWidth4;
         for (j = 0; j < biWidth; j++)
         {
            data2[(biHeight - j - 1) * biWidth4 + i] = data1[k];
            k = k + 1;
         }
      }
   }

   //
   /* �}�ҷs�ɮ� */
   if ((output_file = fopen("triangleRotate90.bmp", "wb")) == NULL)
   {
      fprintf(stderr, "Output file can't open.\n");
      exit(0);
   }

   fwrite(bmpfileheader1, sizeof(bmpfileheader1), 1, output_file);
   fwrite(bmpinfoheader1, sizeof(bmpinfoheader1), 1, output_file);

   fwrite(color_table1, 1024, 1, output_file);

   fwrite(data2, ((biWidth * 1 + 3) / 4 * 4) * biHeight * 1, 1, output_file);

   fclose(output_file);

   free(data1);
   free(data2);

   return 0;
}

int ReadDataSize(char *name)
{
   FILE *input_file = 0;
   U_CHAR bmpfileheader[14] = {0};
   U_CHAR bmpinfoheader[40] = {0};

   INT32 biWidth = 0;
   INT32 biHeight = 0;
   INT16 BitCount = 0;

   /* �}���ɮ� */
   if ((input_file = fopen(name, "rb")) == NULL)
   {
      fprintf(stderr, "File can't open.\n");
      exit(0);
   }

   FREAD(input_file, bmpfileheader, 14);
   FREAD(input_file, bmpinfoheader, 40);

   if (GET_2B(bmpfileheader, 0) == 0x4D42) /* 'BM' */
      fprintf(stdout, "BMP file.\n");
   else
   {
      fprintf(stdout, "Not bmp file.\n");
      exit(0);
   }

   biWidth = GET_4B(bmpinfoheader, 4);
   biHeight = GET_4B(bmpinfoheader, 8);
   BitCount = GET_2B(bmpinfoheader, 14);

   if (BitCount != 8)
   {
      fprintf(stderr, "Not a 8-bit file.\n");
      fclose(input_file);
      exit(0);
   }

   //
   fclose(input_file);

   return ((biWidth * 1 + 3) / 4 * 4) * biHeight * 1;
}

void ReadImageData(char *name, U_CHAR *bmpfileheader, U_CHAR *bmpinfoheader, U_CHAR *color_table, U_CHAR *data)
{
   FILE *input_file = 0;

   INT32 FileSize = 0;
   INT32 bfOffBits = 0;
   INT32 headerSize = 0;
   INT32 biWidth = 0;
   INT32 biHeight = 0;
   INT16 biPlanes = 0;
   INT16 BitCount = 0;
   INT32 biCompression = 0;
   INT32 biImageSize = 0;
   INT32 biXPelsPerMeter = 0, biYPelsPerMeter = 0;
   INT32 biClrUsed = 0;
   INT32 biClrImp = 0;

   /* �}���ɮ� */
   if ((input_file = fopen(name, "rb")) == NULL)
   {
      fprintf(stderr, "File can't open.\n");
      exit(0);
   }

   FREAD(input_file, bmpfileheader, 14);
   FREAD(input_file, bmpinfoheader, 40);

   if (GET_2B(bmpfileheader, 0) == 0x4D42) /* 'BM' */
      fprintf(stdout, "BMP file.\n");
   else
   {
      fprintf(stdout, "Not bmp file.\n");
      exit(0);
   }

   FileSize = GET_4B(bmpfileheader, 2);
   bfOffBits = GET_4B(bmpfileheader, 10);
   headerSize = GET_4B(bmpinfoheader, 0);
   biWidth = GET_4B(bmpinfoheader, 4);
   biHeight = GET_4B(bmpinfoheader, 8);
   biPlanes = GET_2B(bmpinfoheader, 12);
   BitCount = GET_2B(bmpinfoheader, 14);
   biCompression = GET_4B(bmpinfoheader, 16);
   biImageSize = GET_4B(bmpinfoheader, 20);
   biXPelsPerMeter = GET_4B(bmpinfoheader, 24);
   biYPelsPerMeter = GET_4B(bmpinfoheader, 28);
   biClrUsed = GET_4B(bmpinfoheader, 32);
   biClrImp = GET_4B(bmpinfoheader, 36);

   printf("FileSize = %ld \n"
          "DataOffset = %ld \n"
          "HeaderSize = %ld \n"
          "Width = %ld \n"
          "Height = %ld \n"
          "Planes = %d \n"
          "BitCount = %d \n"
          "Compression = %ld \n"
          "ImageSize = %ld \n"
          "XpixelsPerM = %ld \n"
          "YpixelsPerM = %ld \n"
          "ColorsUsed = %ld \n"
          "ColorsImportant = %ld \n",
          FileSize, bfOffBits, headerSize, biWidth, biHeight, biPlanes,
          BitCount, biCompression, biImageSize, biXPelsPerMeter, biYPelsPerMeter, biClrUsed, biClrImp);

   if (BitCount != 8)
   {
      fprintf(stderr, "Not a 8-bit file.\n");
      fclose(input_file);
      exit(0);
   }

   FREAD(input_file, color_table, 1024);

   //

   //
   fseek(input_file, bfOffBits, SEEK_SET);
   FREAD(input_file, data, ((biWidth * 1 + 3) / 4 * 4) * biHeight * 1);
   //
   fclose(input_file);
}
