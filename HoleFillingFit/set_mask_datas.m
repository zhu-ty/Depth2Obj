function [img] = set_mask_datas(img,mask,r)

    w = size(mask,2);
    h = size(mask,1);
    ind = find(mask ~= 0);
    [y,x] = ind2sub([h,w],ind);
    for i = 1:size(x,1)
        img(y(i),x(i)) =  1.0 / (r(1) * x(i) + r(2) * y(i) + r(3));
    end
end

