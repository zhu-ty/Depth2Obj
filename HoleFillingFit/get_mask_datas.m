function [data] = get_mask_datas(img,mask)

    w = size(mask,2);
    h = size(mask,1);
    ind = find(mask ~= 0);
    [x,y] = ind2sub([h,w],ind);
    dd = img(ind);
    data = [x,y,dd];
end

