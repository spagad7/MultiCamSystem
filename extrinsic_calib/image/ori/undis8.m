
for id = 1:8
    sid = sprintf('%07d',id);
    UndistortImageRadial1(['image' sid '.jpg'], ['../../calib/calib' sid '.txt'], ['../image' sid '.jpg'])
end
