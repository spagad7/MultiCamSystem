function UndistortImageRadial(filename, filename_calib, filename_out)

% filename = '0_119.jpg';
% filename_calib = 'calib00.txt';


[K, k1, k2] = LoadCalibration(filename_calib);
fx = K(1,1);
fy = K(2,2);
px = K(1,3);
py = K(2,3);

im = imread(filename);
im = double(im);

[X, Y] = meshgrid(1:(size(im,2)), 1:(size(im,1)));
h = size(X, 1); w = size(X,2);

X_n = (X-px)/fx;
Y_n = (Y-py)/fy;
r_u = sqrt(X_n.^2+Y_n.^2);

L = 1 + k1 * r_u.^2 + k2 * r_u.^4;

X_dist_n = X_n.* L;
Y_dist_n = Y_n.* L;

X_dist = X_dist_n*fx + px;
Y_dist = Y_dist_n*fy + py;

imUndistortion(:,:,1) = reshape(interp2(im(:,:,1), X_dist(:), Y_dist(:)), [h, w]);
% imUndistortion(:,:,2) = reshape(interp2(im(:,:,2), X_dist(:), Y_dist(:)), [h, w]);
% imUndistortion(:,:,3) = reshape(interp2(im(:,:,3), X_dist(:), Y_dist(:)), [h, w]);

imUndistortion = uint8(imUndistortion);
figure(1)
clf;
imshow(imUndistortion);
imwrite(imUndistortion, filename_out);

function [K, k1, k2] = LoadCalibration(filename)

fid = fopen(filename);
fscanf(fid, '%s', 5);
fx = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
fy = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
px = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
py = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
k1 = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
k2 = fscanf(fid, '%f', 1);
fclose(fid);
K = eye(3,3);
K(1,1) = fx;
K(2,2) = fy;
K(1,3) = px;
K(2,3) = py;