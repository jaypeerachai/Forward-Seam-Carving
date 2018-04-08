/* Forward Seam Carving
   Project for MultiMedia Course
   Thanachanok Siripitakul Section 1 ID: 5988036
   Ravikan Thanapanaphruekkul Section 1 ID: 5988046
   Peerachai Banyongrakkul Section 1 ID: 5988070
   Sakunrat Nunthavanich Section 1 ID: 5988095
   Pawin Sudjaidee Section 1 ID: 5988222
*/
#include "stdio.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main() {

	Mat img = imread("7.jpg", IMREAD_COLOR); resize(img, img, Size(650, 400));
	namedWindow("ORIGINAL", WINDOW_AUTOSIZE);
	imshow("ORIGINAL", img);
	int c = cvWaitKey(0);
	Size sz = img.size();
	int height = sz.height;
	int width = sz.width;

	//cout << "ORIGINAL \n\n" << img << "\n\n\n\n";

	// Esc = 27, a = 97, d = 100, s = 115, w = 119
	while (c != 27)
	{
		Mat gsimg;
		// convert original color image to grayscale image
		cvtColor(img, gsimg, COLOR_BGR2GRAY);

		// cout << "ORIGINAL GRAYSCALE: \n\n" << gsimg << "\n\n\n\n";

		// initialize arrays which keep best seam
		int kMatCol[5000];
		int mMatCol[5000];
		int Column[5000];
		int kMatRow[5000];
		int mMatRow[5000];
		int Row[5000];

		// initialize matrix M and K in type int
		Mat M = (Mat_<int>(height, width));
		Mat K = (Mat_<int>(height, width));

		/********************************************************************** Duplicate Padding **********************************************************************/
		Mat dst(height + 2, width + 2, CV_8UC1, Scalar(0));
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				dst.at<uchar>(i + 1, j + 1) = gsimg.at<uchar>(i, j);
			}
		}
		// duplicate first column and last column
		for (int i = 0; i < height; i++)
		{
			dst.at<uchar>(i + 1, 0) = gsimg.at<uchar>(i, 0);
			dst.at<uchar>(i + 1, width + 1) = gsimg.at<uchar>(i, width - 1);
		}
		// duplicate first row and last row
		for (int j = 0; j < width; j++)
		{
			dst.at<uchar>(0, j + 1) = gsimg.at<uchar>(0, j);
			dst.at<uchar>(height + 1, j + 1) = gsimg.at<uchar>(height - 1, j);
		}
		// duplicate 4 edges 
		dst.at<uchar>(0, 0) = gsimg.at<uchar>(0, 0);
		dst.at<uchar>(0, width + 1) = gsimg.at<uchar>(0, width - 1);
		dst.at<uchar>(height + 1, 0) = gsimg.at<uchar>(height - 1, 0);
		dst.at<uchar>(height + 1, width + 1) = gsimg.at<uchar>(height - 1, width - 1);

		//imshow("PADDING", dst);
		//cout << "PADDING \n\n" << dst << "\n\n\n\n";

		/********************************************************************** End Duplicate Padding **********************************************************************/


		// Looping till get the command 'a','d','w','s'
		while (c != 97 && c != 100 && c != 115 && c != 119 && c != 27)
		{
			c = cvWaitKey(0);
		}

		/********************************************************************** VERTICAL **********************************************************************/

		// keyboard command ::: 'a' and 'd' => vertical best seam, 'w' and 's' => horizontal best seam
		// 'a' => Reduce width, d => increase width
		if (c == 97 || c == 100)
		{
			// Construct M matrix and K matrix in the vertical direction
			// *** WRITE YOUR CODE ***
			for (int h = 0; h < dst.rows-2; h++)
			{
				for (int w = 0; w < dst.cols-2; w++)
				{
					// in case of first row
					if (h == 0)
					{ 
						M.at<int>(h, w) = abs(dst.at<uchar>(h, w) - dst.at<uchar>(h, w + 2));
						// all elements in first row of K matrix are 0
						K.at<int>(h, w) = 0;
					}
					else 
					{
						// in case of middle element, compare 1+cl, 2+cu, and 3+cr to find min
						if (w != 0 && w != dst.cols - 3)
						{
							// calculate cl, cu, and cr
							int cl = abs(dst.at<uchar>(h, w + 1) - dst.at<uchar>(h + 1, w)) + abs(dst.at<uchar>(h + 1, w + 2) - dst.at<uchar>(h + 1, w));
							int cu = abs(dst.at<uchar>(h + 1, w + 2) - dst.at<uchar>(h + 1, w));
							int cr = abs(dst.at<uchar>(h, w + 1) - dst.at<uchar>(h + 1, w + 2)) + abs(dst.at<uchar>(h + 1, w + 2) - dst.at<uchar>(h + 1, w));
							//find minimum number and keep in M matrix
							M.at<int>(h, w) = min({ M.at<int>(h - 1 , w - 1) + cl, M.at<int>(h - 1 , w) + cu, M.at<int>(h - 1 , w + 1) + cr });
							//keep 1 or 2 or 3 in K matrix
							int a = min({ M.at<int>(h - 1 , w - 1) + cl, M.at<int>(h - 1 , w) + cu, M.at<int>(h - 1 , w + 1) + cr });
							if (a == (M.at<int>(h - 1, w - 1) + cl))
							{
								K.at<int>(h, w) = 1;
							}
							else if (a == (M.at<int>(h - 1, w) + cu))
							{
								K.at<int>(h, w) = 2;
							}
							else
							{
								K.at<int>(h, w) = 3;
							}
						}
						// in case of the first element in each row, only compare 2+cu and 3+cr to find min
						else if (w==0)
						{
							int cu = abs(dst.at<uchar>(h, w) - dst.at<uchar>(h , w + 2));
							int cr = abs(dst.at<uchar>(h-1, w ) - dst.at<uchar>(h, w + 1)) + abs(dst.at<uchar>(h , w ) - dst.at<uchar>(h , w + 1));
							M.at<int>(h, w) = min( M.at<	int>(h - 1, w) + cu, M.at<int>(h - 1, w + 1) + cr );
							//keep 2 or 3 in K matrix
							int a = min(M.at<int>(h - 1 , w) + cu, M.at<int>(h - 1 , w + 1) + cr );
							if (a == (M.at<int>(h - 1, w) + cu))
							{
								K.at<int>(h, w) = 2;
							}
							else
							{
								K.at<int>(h, w) = 3;
							}
						}
						// in case of the last element in each row, only compare 1+cl and 2+cu to find min
						else if (w == dst.cols - 3)
						{
							int cl = abs(dst.at<uchar>(h+1, w) - dst.at<uchar>(h+1, w+2)) + abs(dst.at<uchar>(h + 1, w + 2) - dst.at<uchar>(h , w+1 ));
							int cu = abs(dst.at<uchar>(h+1, w) - dst.at<uchar>(h+1, w+2));
							M.at<int>(h, w) = min(M.at<int>(h-1, w-1 ) + cl, M.at<int>(h-1, w)+cu);
							//keep 1 or 2 in K matrix
							int a = min(M.at<int>(h - 1 , w - 1) + cl, M.at<int>(h - 1 , w) + cu);
							if (a == (M.at<int>(h - 1, w) + cu))
							{
								K.at<int>(h, w) = 1;
							}
							else
							{
								K.at<int>(h, w) = 2;
							}
						}
					}
				}
			}
			
			// Find the best seam in the vertical direction
			// *** WRITE YOUR CODE ***
			int min = M.at<int>(M.rows-1, 0);
			int wLow = 1;
			int c = 0;
			for (int w = 1; w < M.cols; w++)
			{
				if (M.at<int>(M.rows-1, w) < min)
				{
					min= M.at<int>(M.rows-1, w);
					wLow = w;
				}
			}

			int sCol = wLow;
			int kCol;
			int mCol;
			int sRow = M.rows - 1;
			int count = 0;
			while (count != M.rows - 1)
			{
				kCol = K.at<int>(sRow, sCol);
				mCol = M.at<int>(sRow, sCol);
				kMatCol[count] = kCol;
				mMatCol[count] = mCol;
				Column[count] = sCol;
				count++;
				sRow--;
				if (kCol == 1 && sCol != 0)
				{
					kCol = sCol-1;
				}
				else if (kCol == 2)
				{
					kCol = sCol;
				}
				else if (kCol == 3 && sCol != M.cols - 1)
				{
					kCol = sCol+1;
				}
				sCol = kCol;
				// keep columns which are a part of best seam in each row into array "Column"
				if (count == M.rows - 1)
				{
					mCol = M.at<int>(sRow, sCol);
					mMatCol[count] = mCol;
					Column[count] = sCol;
				}
			}
		}
		/********************************************************************** END VERTICAL **********************************************************************/

		/********************************************************************** HORIZONTAL **********************************************************************/

		// 'w' => increase height, 's' => reduce height
		if (c == 115 || c == 119)
		{
			// Construct M matrix and K matrix in the horizontal direction
			// *** WRITE YOUR CODE ***
			for (int w = 0; w < dst.cols - 2; w++)
			{
				for (int h = 0; h < dst.rows - 2; h++)
				{
					// in case of first column
					if (w == 0)
					{
						M.at<int>(h, w) = abs(dst.at<uchar>(h, w + 1) - dst.at<uchar>(h + 2, w + 1));
						// all elements in first column of K matrix are 0
						K.at<int>(h, w) = 0;
					}
					else
					{
						// in case of middle element, compare 1+cl, 2+cu, and 3+cr to find min
						if (h != 0 && h != dst.rows - 3)
						{
							// calculate cl, cu, and cr
							int cl = abs(dst.at<uchar>(h + 1, w) - dst.at<uchar>(h + 2, w + 1)) + abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h, w + 1));
							int cu = abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h, w + 1));
							int cr = abs(dst.at<uchar>(h + 1, w) - dst.at<uchar>(h, w + 1)) + abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h, w + 1));
							// find minimum number and keep in M matrix
							M.at<int>(h, w) = min({ M.at<int>(h + 1 , w - 1) + cl, M.at<int>(h, w - 1) + cu, M.at<int>(h - 1 , w - 1) + cr });
							//keep 1 or 2 or 3 in K matrix
							int a = min({ M.at<int>(h + 1 , w - 1) + cl, M.at<int>(h, w - 1) + cu, M.at<int>(h - 1 , w - 1) + cr });
							if (a == (M.at<int>(h + 1, w - 1) + cl))
							{
								K.at<int>(h, w) = 1;
							}
							else if (a == (M.at<int>(h, w - 1) + cu))
							{
								K.at<int>(h, w) = 2;
							}
							else
							{
								K.at<int>(h, w) = 3;
							}
						}
						// in case of the first element in each column, only compare 2+cu and 1+cl to find min
						else if (h == 0)
						{
							int cu = abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h, w + 1));
							int cl = abs(dst.at<uchar>(h, w + 1) - dst.at<uchar>(h + 2, w + 1)) + abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h + 1, w));
							M.at<int>(h, w) = min(M.at<int>(h, w - 1) + cu, M.at<int>(h + 1, w - 1) + cl);
							//keep 2 or 3 in K matrix
							int a = min(M.at<int>(h, w - 1) + cu, M.at<int>(h + 1, w - 1) + cl);
							if (a == (M.at<int>(h, w - 1) + cu))
							{
								K.at<int>(h, w) = 2;
							}
							else
							{
								K.at<int>(h, w) = 1;
							}
						}
						// in case of the last element in each row, only compare 3+cr and 2+cu to find min
						else if (h == dst.rows - 3)
						{
							int cr = abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h, w + 1)) + abs(dst.at<uchar>(h + 1, w) - dst.at<uchar>(h, w + 1));
							int cu = abs(dst.at<uchar>(h + 2, w + 1) - dst.at<uchar>(h, w + 1));
							M.at<int>(h, w) = min(M.at<int>(h - 1, w - 1) + cr, M.at<int>(h, w - 1) + cu);
							//keep 2 or 3 in K matrix
							int a = min(M.at<int>(h - 1, w - 1) + cr, M.at<int>(h, w - 1) + cu);
							if (a == (M.at<int>(h - 1, w - 1) + cr))
							{
								K.at<int>(h, w) = 3;
							}
							else
							{
								K.at<int>(h, w) = 2;
							}
						}
					}
				}
			}

			// Find the best seam in the horizontal direction
			// *** WRITE YOUR CODE ***
			int min = M.at<int>(0, M.cols - 1);
			int hLow = 1;
			int c = 0;
			for (int h = 1; h < M.rows; h++)
			{
				if (M.at<int>(h, M.cols - 1) < min)
				{
					min = M.at<int>(h, M.cols - 1);
					hLow = h;
				}
			}

			int sRow = hLow;
			int kRow;
			int mRow;
			int sCol = M.cols - 1;
			int count = 0;
			while (count != M.cols - 1)
			{
				kRow = K.at<int>(sRow, sCol);
				mRow = M.at<int>(sRow, sCol);
				kMatRow[count] = kRow;
				mMatRow[count] = mRow;
				Row[count] = sRow;
				count++;
				sCol--;
				if (kRow == 1 && sRow != M.rows - 1)
				{
					kRow = sRow + 1;
				}
				else if (kRow == 2)
				{
					kRow = sRow;
				}
				else if (kRow == 3 && sRow != 0)
				{
					kRow = sRow - 1;
				}
				sRow = kRow;
				// keep rows which are a part of best seam in each column into array "Row"
				if (count == M.cols - 1)
				{
					mRow = M.at<int>(sRow, sCol);
					mMatRow[count] = mRow;
					Row[count] = sRow;
				}
			}
		}
		/********************************************************************** END HORIZONTAL **********************************************************************/

		// Insert or delete the best seam
		if (c == 97)
		{
			//Reduce width or delete seam vertically
			// Copy the pixels into this image
			Mat img_new(height, --width, CV_8UC3, Scalar(0, 0, 0));
			// *** WRITE YOUR CODE ***
			int colsIndex = height - 1;
			int OimgWidth = 0;
			for (int h = 0; h < img_new.rows; h++)
			{
				for (int w = 0; w < img_new.cols; w++)
				{
					// remove best seam
					if (w == Column[colsIndex])
					{
						OimgWidth++;
					}
					img_new.at<Vec3b>(h, w) = img.at<Vec3b>(h, OimgWidth);
					OimgWidth++;
				}
				colsIndex--;
				OimgWidth = 0;
			}

			//Show the resized image
			imshow("Example", img_new);

			img.release();
			// Clone img_new into img for the next loop processing
			img = img_new.clone();
			img_new.release();
		}

		if (c == 100)
		{
			//Increase width or insert seam vertically
			// Copy the pixels into this image
			Mat img_new(height, ++width, CV_8UC3, Scalar(0, 0, 0));
			// *** WRITE YOUR CODE ***
			int max = height-1;
			int ww = 0;
			for (int h = 0; h < height; h++)
			{
				for (int w = 0; w < width; w++)
				{
					// duplicate best seam
					if (w == Column[max])
					{
						img_new.at<Vec3b>(h, w) = img.at<Vec3b>(h, ww);
						img_new.at<Vec3b>(h, w+1) = img.at<Vec3b>(h, ww);
						w++;
						ww++;
					}
					else
					{
						img_new.at<Vec3b>(h, w) = img.at<Vec3b>(h, ww);
						ww++;
					}
				}
				ww = 0;
				max--;
			}

			//Show the resized image
			imshow("Example", img_new);

			img.release();
			// Clone img_new into img for the next loop processing
			img = img_new.clone();
			img_new.release();
		}
		if (c == 115) {
			//Reduce height or delete seam horizontally
			// Copy the pixels into this image
			Mat img_new(--height, width, CV_8UC3, Scalar(0, 0, 0));
			// *** WRITE YOUR CODE ***

			int rowsIndex = width - 1;
			int OimgHeight = 0;
			for (int w = 0; w < img_new.cols; w++)
			{
				for (int h = 0; h < img_new.rows; h++)
				{
					// remove best seam
					if (h == Row[rowsIndex])
					{
						OimgHeight++;
					}
					img_new.at<Vec3b>(h, w) = img.at<Vec3b>(OimgHeight, w);
					OimgHeight++;
				}

				rowsIndex--;
				OimgHeight = 0;
			}

			//Show the resized image
			imshow("Example", img_new);
			// Clone img_new into img for the next loop processing
			img.release();
			img = img_new.clone();
			img_new.release();
		}
		if (c == 119) {
			//Increase height or insert seam horizontally
			//Copy the pixels into this image
			Mat img_new(++height, width, CV_8UC3, Scalar(0, 0, 0));
			int max = width - 1;
			int hh = 0;
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					// duplicate best seam
					if (h == Row[max])
					{
						img_new.at<Vec3b>(h, w) = img.at<Vec3b>(hh, w);
						img_new.at<Vec3b>(h+1, w) = img.at<Vec3b>(hh, w);
						h++;
						hh++;
					}
					else
					{
						img_new.at<Vec3b>(h, w) = img.at<Vec3b>(hh, w);
						hh++;
					}
				}
				hh = 0;
				max--;
			}

			//***Write your code***

			//Show the resized image
			imshow("Example", img_new);
			//Clone img_new into img for the next loop processing
			img.release();
			img = img_new.clone();
			img_new.release();
		}

		if (c == 27 || height == 1 || width == 1)
			break;
		c = cvWaitKey(0);
	}
	return 0;
}