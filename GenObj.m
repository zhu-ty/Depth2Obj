function [] = GenObj(depth, color_path, objname, t)
%GENOBJ �������ͼ���ɫͼ·�������obj��mtl
    Id = depth;
    y_max = size(1:t:size(Id,1),2);
    x_max = size(1:t:size(Id,2),2);

    file = fopen([objname,'.obj'],'w');
    idx = strfind(objname,'/');
    if(~isempty(idx))
        objname_end = objname(idx(end)+1:end);
    else
        objname_end = objname;
    end
    
    depth_list = zeros(round(size(Id,2) * size(Id,1) / (t*t)), 1);
    id = 1;
    
    fprintf(file,['mtllib ',objname_end,'.mtl\nusemtl 01___Default\n']);
    for x = 1:t:size(Id,2)
        for y = 1:t:size(Id,1)
            fprintf(file,'v %f %f %f\n',x,-y,-1 * double(Id(y,x)));
            fprintf(file,'vt %f %f %f\n',x/size(Id,2),1.0 - y/size(Id,1),0.000);
            depth_list(id) = double(Id(y,x));
            id = id +1;
        end
    end
    
    idx = 1;
    while(1)
        if(idx + y_max + 1> y_max * x_max)
            break;
        elseif(mod(idx,y_max) ~= 0)
            if(depth_list(idx) > 0 &&  depth_list(idx + y_max + 1) > 0 && depth_list(idx + y_max) > 0)
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
            if(depth_list(idx) > 0 &&  depth_list(idx + y_max + 1) > 0 && depth_list(idx + 1) > 0)
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

