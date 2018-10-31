d2 = imread('mask_toinpaint.png');
imshow(d2,[]);

I = imread('desk_1_82.png');
J = I;
J(:,:,1) = regionfill(I(:,:,1), d2);
J(:,:,2) = regionfill(I(:,:,2), d2);
J(:,:,3) = regionfill(I(:,:,3), d2);
imshow(J)
%imwrite(J,'fill_color.jpg');

% 
% K = I;
% T = I(:,:,1);
% T(d2 ~= 0) = 0;
% K(:,:,1) = T;
% T = I(:,:,2);
% T(d2 ~= 0) = 0;
% K(:,:,2) = T;
% T = I(:,:,3);
% T(d2 ~= 0) = 0;
% K(:,:,3) = T;
% imshow(K)