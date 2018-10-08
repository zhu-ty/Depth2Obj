
depth_p = 'real/depth_DepthOnly.png';


I_depth = imread(depth_p);
object = 5;
target_region = logical(zeros(size(I_depth,1),size(I_depth,2)));

imshow(log(double(I_depth)),[]);
I_depth_show = double(I_depth);

for i = 1:object
    [x, y] = ginput;
    target_region_tmp = poly2mask(x, y, size(I_depth,1), size(I_depth, 2));
    target_region = target_region | target_region_tmp;
    
    I_depth_show = double(I_depth) .* (1 - 0.2 * double(target_region));
    imshow(log(I_depth_show),[]);
end

imwrite(target_region,'real/mask_toinpaint.png');

system('.\HoleFilling.exe real\depth_DepthOnly.png real\mask_toinpaint.png real\depth_inpainted.png');