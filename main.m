clear;
clc;
% Id = imread('desk/output_ns.png');
% GenObj(Id,'desk/desk_1_82.png','desk/inpainted_ns',2);

% Id = imread('desk/output_depth_hole_filled.png');
% GenObj(Id,'desk/desk_1_82.png','desk/origin',2);


Id = imread('desk/output_depth_hole_filled.png');
Im = imread('desk/mask_toinpaint.png');
GenObjectObj(Id,'desk/desk_1_82.png','desk/origin_object_t_1',1, Im, 10);

% Id = double(imread('real/depth_DepthOnly.png'));
% Id = filter2(fspecial('average',3),Id);
% GenObj(Id,'real/0000_left.png','real/origin',2);

% Id = double(imread('real/depth_inpainted.png'));
% Id = filter2(fspecial('average',3),Id);
% GenObj(Id,'real/0000_left.png','real/__inpainted__',2);

% Id = double(imread('real/depth_inpainted.png'));
% Id = filter2(fspecial('average',3),Id);
% GenObjectObj(Id,'real/0000_left.png','real/__inpainted__with_threshold',2, ones(size(Id,1),size(Id,2)), 50);