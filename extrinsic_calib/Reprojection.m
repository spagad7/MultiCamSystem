function Reprojection

X = LoadStructure('reconstruction/structure.txt');
C = LoadCamera('reconstruction/camera.txt');

figure(10)
clf;
plot3(X(:,1), X(:,2), X(:,3), 'b.');
for i = 1 : length(C)
    hold on
    DisplayCamera(C(i).C, C(i).R, 0.5);
end
return;

for i = 1 : length(C)
    str = sprintf('calib/calib%07d.txt', C(i).frame);
    K = LoadCalib(str);
    P = K * C(i).R * [eye(3) -C(i).C];
    im_str = sprintf('image/image%07d.jpg', C(i).frame+1);
    u = P * [X'; ones(1, size(X,1))];
    u = [u(1,:)./u(3,:); u(2,:)./u(3,:)];
    
    figure(i)
    clf;
    imshow(imread(im_str));
    hold on
    plot(u(1,:), u(2,:), 'rx');
end

function X = LoadStructure(filename)
fid = fopen(filename);
fscanf(fid, '%s', 1);
n = fscanf(fid, '%d', 1);
X = fscanf(fid, '%f', [7 n])';
X = X(:,5:7);
fclose(fid);

function K = LoadCalib(filename)
fid = fopen(filename);
fscanf(fid, '%s', 5);
K = eye(3);
K(1,1) = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
K(2,2) = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
K(1,3) = fscanf(fid, '%f', 1);
fscanf(fid, '%s', 1);
K(2,3) = fscanf(fid, '%f', 1);
fclose(fid);

function C = LoadCamera(filename)
fid = fopen(filename);
fscanf(fid, '%s', 5);
n = fscanf(fid, '%d', 1);
for i = 1 : n
    fscanf(fid, '%d', 1);
    C(i).frame = fscanf(fid, '%d', 1);
    C(i).C = fscanf(fid, '%f', [1 3])';
    C(i).R = fscanf(fid, '%f', [3 3])';    
end
fclose(fid);