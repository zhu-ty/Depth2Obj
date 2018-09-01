clear;
clc;
% Id = imread('desk/output_ns.png');
% GenObj(Id,'desk/desk_1_82.png','desk/inpainted_ns',2);

% Id = imread('desk/output_depth_hole_filled.png');
% GenObj(Id,'desk/desk_1_82.png','desk/origin',2);


Id = imread('desk/output_depth_hole_filled.png');
Im = imread('desk/mask_toinpaint.png');
GenObjectObj(Id,'desk/desk_1_82.png','desk/origin_object',2, Im, 10);