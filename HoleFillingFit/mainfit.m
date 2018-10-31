clear;
clc;

mask = imread('../desk/mask_toinpaint.png');
depth = imread('../desk/depth_all.png');

depth_dst = double(depth);

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
    
    data2 = double(data);
    xxn = data2(:,1:2);
    yyn = data2(:,3);
    %try 1/d=ax+by+c
    yyn1 = 1 ./ yyn;
    r = regress(yyn1,[xxn,ones(size(xxn,1),1)]);
    r2 = robustfit(xxn,yyn1);
    r2 = circshift(r2,-1);
    depth_dst = set_mask_datas(depth_dst, mask_split(:,:,i), r2);
end

figure(1);imshow(depth .* uint16(~mask),[]);
figure(2);imshow(depth_dst,[]);
imwrite(uint16(depth_dst), 'test_fill_depth.png');




