
depth_p = 'desk/output_depth_hole_filled.png';


I_depth = imread(depth_p);
object = 2;
target_region = logical(zeros(size(I_depth,1),size(I_depth,2)));

imshow(log(double(I_depth)),[]);

for i = 1:object
    [x, y] = ginput;
    target_region_tmp = poly2mask(x, y, size(I_depth,1), size(I_depth, 2));
    target_region = target_region | target_region_tmp;
end

imwrite(target_region,'desk/mask_toinpaint.png');

system('desk\HoleFilling.exe desk\output_depth_hole_filled.png desk\mask_toinpaint.png desk\depth_inpainted.png');