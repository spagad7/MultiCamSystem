
for id = 1:8
    sid = sprintf('%07d',id);
    UndistortImageRadial1(['image' sid '.jpg'], ['../../calib/calib' sid '.txt'], ['../image' sid '.jpg'])
end
% UndistortImageRadial1('image0000001.jpg', '../../calib/calib0000001.txt', '../image0000001.jpg')
% UndistortImageRadial1('image0000002.jpg', '../../calib/calib0000002.txt', '../image0000002.jpg')
% UndistortImageRadial1('image0000003.jpg', '../../calib/calib0000003.txt', '../image0000003.jpg')
% UndistortImageRadial1('image0000004.jpg', '../../calib/calib0000004.txt', '../image0000004.jpg')
% UndistortImageRadial1('image0000005.jpg', '../../calib/calib0000005.txt', '../image0000005.jpg')
% UndistortImageRadial1('image0000006.jpg', '../../calib/calib0000006.txt', '../image0000006.jpg')
% UndistortImageRadial1('image0000007.jpg', '../../calib/calib0000007.txt', '../image0000007.jpg')
% UndistortImageRadial1('image0000008.jpg', '../../calib/calib0000008.txt', '../image0000008.jpg')