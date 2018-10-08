clear;
clc;

mask = imread('../desk/mask_toinpaint.png');
depth = imread('../desk/depth_all.png');

w = size(mask,2);
h = size(mask,1);

CC = bwconncomp(mask);
object_num = size(CC.PixelIdxList,2);
mask_split = logical(zeros(h,w,object_num));
for i = 1:object_num
    mask_tmp = logical(zeros(h,w));
    mask_tmp(CC.PixelIdxList{i}) = 1;
    mask_split(:,:,i) = mask_tmp;
    
    B = ones(15,15);
    A2 = imdilate(mask_split(:,:,i),B);
    A3 = A2 - mask_split(:,:,i);
    %imshow(A3,[]);
    %ginput(1);
    A4 = A3 & (~mask);
    %imshow(A4,[]);
    %ginput(1);
    
    data = get_mask_datas(depth,A4);
end



