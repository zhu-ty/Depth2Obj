function [] = GenObjectObj(depth, color_path, objname, t,object_mask, depth_threshold)

Id = depth;
    y_max = size(Id,1) / t;
    x_max =  size(Id,2) / t;

    file = fopen([objname,'.obj'],'w');
    idx = strfind(objname,'/');
    if(~isempty(idx))
        objname_end = objname(idx(end)+1:end);
    else
        objname_end = objname;
    end
    
    v_list = zeros(round(size(Id,2) * size(Id,1) / (t*t)), 3); %x,y,d
    id = 1;
    
    fprintf(file,['mtllib ',objname_end,'.mtl\nusemtl 01___Default\n']);
    for x = 1:t:size(Id,2)
        for y = 1:t:size(Id,1)
            %if(object_mask(y,x) > 0)
            fprintf(file,'v %f %f %f\n',x,-y,-1 * double(Id(y,x)));
            fprintf(file,'vt %f %f %f\n',x/size(Id,2),1.0 - y/size(Id,1),0.000);
            v_list(id, :) = double([x,y,Id(y,x)]);
            id = id +1;
            %end
        end
    end
    
    idx = 1;
    while(1)
        if(idx + y_max + 1> y_max * x_max)
            break;
        elseif(mod(idx,y_max) ~= 0)
            pt1 = v_list(idx,:);
            pt2 = v_list(idx + y_max + 1,:);
            pt3 = v_list(idx + y_max,:);
            if(pt1(3) > 0 &&  pt2(3) > 0 && pt3(3) > 0 ...
                  && object_mask(pt1(2),pt1(1)) > 0 && object_mask(pt2(2),pt2(1)) > 0 && object_mask(pt3(2),pt3(1)) > 0 ...
                  && abs(pt1(3) - pt2(3)) < depth_threshold &&abs(pt2(3) - pt3(3)) < depth_threshold &&abs(pt1(3) - pt3(3)) < depth_threshold)
                fprintf(file,'f %d/%d %d/%d %d/%d\n',idx,idx, idx + y_max + 1, idx + y_max + 1, idx + y_max,idx + y_max);
            end
        end
        idx = idx + 1;
    end
    idx = 1;
    while(1)
        if(idx + y_max + 1> y_max * x_max)
            break;
        elseif(mod(idx,y_max) ~= 0)
            pt1 = v_list(idx,:);
            pt2 = v_list(idx + 1,:);
            pt3 = v_list(idx + y_max + 1,:);
            if(pt1(3) > 0 &&  pt2(3) > 0 && pt3(3) > 0 ...
                  && object_mask(pt1(2),pt1(1)) > 0 && object_mask(pt2(2),pt2(1)) > 0 && object_mask(pt3(2),pt3(1)) > 0 ...
                  && abs(pt1(3) - pt2(3)) < depth_threshold &&abs(pt2(3) - pt3(3)) < depth_threshold &&abs(pt1(3) - pt3(3)) < depth_threshold)
                fprintf(file,'f %d/%d %d/%d %d/%d\n',idx,idx, idx + 1,idx + 1, idx + y_max + 1, idx + y_max + 1);
            end
        end
        idx = idx + 1;
    end
    fclose(file);
    
    idx = strfind(color_path,'/');
    if(~isempty(idx))
        color_path_end = color_path(idx(end)+1:end);
    else
        color_path_end = color_path;
    end
    
    file = fopen([objname,'.mtl'],'w');
    fprintf(file,['newmtl 01___Default \n',...
    'Ns 10.0000\n',...
    'Ni 1.5000\n',...
    'd 1.0000\n',...
    'Tr 0.0000\n',...
    'Tf 1.0000 1.0000 1.0000 \n',...
    'illum 2\n',...
    'Ka 0.5882 0.5882 0.5882\n',...
    'Kd 0.5882 0.5882 0.5882\n',...
    'Ks 0.0000 0.0000 0.0000\n',...
    'Ke 0.0000 0.0000 0.0000\n',...
    'map_Ka ',color_path_end,'\n',...
    'map_Kd ',color_path_end,'\n']);

end

