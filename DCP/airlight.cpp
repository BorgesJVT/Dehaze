#include "dcp_core.h"

void Estimate_A(IplImage *DarkChannel, IplImage *InputImage, struct Max_Pixel_Selector *MaxPixel, double A[])
{
	double num = height * width * 0.001;
	double num1per = num * 0.01;   //num1per is the number of 1 percent of top 0.1 pixels
	//k is the index of the color channel
	int k = 0;
	int patchsize;
	int dark_i, dark_j;
	uchar Max = 0;

	IplImage *mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvZero(mask);

	int e = 0, i, j;
	for (i = 1; i <= height; i++)
	{
		for (j = 1; j <= width; j++)
		{
			MaxPixel[e].Pixel = *(uchar *)(DarkChannel->imageData + (i - 1) * gwidthstep + (j - 1));
			MaxPixel[e].i = i;
			MaxPixel[e].j = j;
			e++;
		}
	}
	sort(MaxPixel, MaxPixel + e, comp1);

	for (i = 0; i < num; i++)
	{
		dark_i = MaxPixel[i].i;
		dark_j = MaxPixel[i].j;
		*(uchar *)(mask->imageData + (dark_i - 1) * gwidthstep + (dark_j - 1)) = 255;
		//ֻ�е�mask���Ϊ255֮�󣬲�˵���ÿ���������ֵ���ڰ�ͨ����ǰ0.1%֮�ڵ�
	}

	if (num1per < 9)
		patchsize = 3;
	else
		patchsize = sqrt(num1per);

	for (k = 0; k < 3; k++)
	{
		int e, x, y;
		int tmp = (patchsize - 1) / 2;
		int st_row, ed_row;
		int st_col, ed_col;

		double A_tmp = 0;
		double aver = 0;  // aver Ϊָ����Сpatch��InputImageֵ��ƽ��ֵ
		int flag;  //flag������ȡmask�ı��

		for (i = 1; i <= num; i++)
		{
			aver = 0;
			dark_i = MaxPixel[i].i;
			dark_j = MaxPixel[i].j;

			st_row = dark_i - tmp, ed_row = dark_i + tmp;
			st_col = dark_j - tmp, ed_col = dark_j + tmp;

			if (st_row <= 0)
				st_row = 1;
			if (ed_row>height)
				ed_row = height;
			if (st_col <= 0)
				st_col = 1;
			if (ed_col>width)
				ed_col = width;

			e = 0;
			for (x = st_row; x <= ed_row; x++)
			{
				for (y = st_col; y <= ed_col; y++)
				{
					flag = *(uchar *)(mask->imageData + (x - 1) * gwidthstep + (y - 1));
					if (flag == 0)   //δ�����˵���õ㲻��ǰ0.1%��Ӧ�������ڣ�����
					{
						continue;
					}
					else
					{
						aver += *(uchar *)(InputImage->imageData + (x - 1) * widthstep + (y - 1) *nch + k);
						e++;
					}
				}
			}
			aver /= e;
			if (A_tmp < aver)
			{
				A_tmp = aver;
			}
		}
		A[k] = A_tmp / 255.0;
	}
	cvReleaseImage(&mask);
}