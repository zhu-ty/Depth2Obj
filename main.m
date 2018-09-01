clear;
clc;
Id = imread('desk/output_depth_hole_filled.png');
GenObj(Id,'desk/desk_1_82.png','desk/origin',2);


% Id = imread('depth.png');
% I_path = 'apple_5_4_220.png';
% 
% t = 2;
% 
% y_max = size(Id,1) /t;
% x_max =  size(Id,2) /t;
% 
% % mesh = zeros(x_max,y_max);
% 
% file = fopen('test3.obj','w');
% 
% fprintf(file,'mtllib test3.mtl\nusemtl 01___Default\n');
% 
% for x = 1:t:size(Id,2)
%     for y = 1:t:size(Id,1)
%         fprintf(file,'v %f %f %f\n',x,1.0 - y,-1 * double(Id(y,x)));
% %         fprintf(file,'v %f %f %f\n',x,y,0.0);
%         fprintf(file,'vt %f %f %f\n',x/size(Id,2),1.0 - y/size(Id,1),0.000);
%     end
% end
% 
% idx = 1;
% while(1)
%     if(idx + y_max + 1> y_max * x_max)
%         break;
%     elseif(mod(idx,y_max) ~= 0)
%         fprintf(file,'f %d/%d %d/%d %d/%d\n',idx,idx, idx + y_max + 1, idx + y_max + 1, idx + y_max,idx + y_max);
%     end
%     idx = idx + 1;
% end
% 
% idx = 1;
% while(1)
%     if(idx + y_max + 1> y_max * x_max)
%         break;
%     elseif(mod(idx,y_max) ~= 0)
%         fprintf(file,'f %d/%d %d/%d %d/%d\n',idx,idx, idx + 1,idx + 1, idx + y_max + 1, idx + y_max + 1);
%     end
%     idx = idx + 1;
% end
% 
% fclose(file);
