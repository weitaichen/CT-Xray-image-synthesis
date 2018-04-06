% info = dicominfo('hum0001.dcm');
% Y = dicomread(info);
% figure
% imshow(Y,[]);

srcFiles = dir('D:\program\matlab\tt\*.dcm');  % the folder in which ur images exists
for i = 1 : length(srcFiles)
    info = strcat('D:\program\matlab\tt\',srcFiles(i).name);
    [Y,MAP]=dicomread(info);
    image8 = uint8(255 * mat2gray(Y));
    imshow(image8,[]);
    info = strcat(info,'.png');
    imwrite(image8 ,info);
end