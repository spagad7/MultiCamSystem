function Visualize3D

C = LoadCamera('reconstruction/camera.txt');
X = LoadStructure('reconstruction/structure.txt');

figure(1)
clf;
plot3(X(:,1), X(:,2), X(:,3), 'b.');
for i = 1 : length(C)
    DisplayCamera(C(i).C, C(i).R, 0.1);
end

function X = LoadStructure(filename)
fid = fopen(filename);
fscanf(fid, '%s', 1);
n = fscanf(fid, '%d', 1);
X = fscanf(fid, '%f', [7 n])';
X = X(:,5:7);
fclose(fid);

function C = LoadCamera(filename)

fid = fopen(filename)
fscanf(fid, '%s', 5);
n = fscanf(fid, '%d', 1);
for i = 1 : n
    fscanf(fid, '%s', 2);
    C(i).C = fscanf(fid, '%f', [1 3])';
    C(i).R = fscanf(fid, '%f', [3 3])';
end
fclose(fid);