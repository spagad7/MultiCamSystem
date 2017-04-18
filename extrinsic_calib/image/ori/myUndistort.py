import numpy as np
import cv2

# myUndistort.py ori_imgFile calibFile out_imgFile

import sys

def undistortImg(ori_imgFile, calibFile, out_imgFile):

	img = cv2.imread(ori_imgFile)
	h,  w = img.shape[:2]

	with open(calibFile) as f:
		caliblines = f.readlines()

	calibsects = [x.split() for x in caliblines]

	print calibsects

	K = np.eye(3)
	disCoeff = np.zeros(4)

	K[0,0] = calibsects[2][1]
	K[1,1] = calibsects[3][1]
	K[0,2] = calibsects[4][1]
	K[1,2] = calibsects[5][1]
	disCoeff[0] = calibsects[6][1]
	disCoeff[1] = calibsects[7][1]
	# print K, disCoeff


	# undistort
	dst = cv2.undistort(img, K, disCoeff)

	# show the images
	cv2.imshow('Original', img)
	cv2.imshow('Undistorted', dst)
	cv2.imwrite(out_imgFile, dst)

	print 'Press any key to exit...'
	cv2.waitKey(0)


if __name__ == '__main__':

	if len(sys.argv) != 4:
		print "3 parameters are needed."
		exit(-1)

	ori_imgFile = sys.argv[1]
	calibFile = sys.argv[2]
	out_imgFile = sys.argv[3]

	undistortImg(ori_imgFile, calibFile, out_imgFile)