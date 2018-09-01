function [] = GenObj(depth, color_path, objname, t)
%GENOBJ 输入深度图与彩色图路径，输出obj与mtl
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
    
    fprintf(file,['mtllib ',objname_end,'.mtl\nusemtl 01___Default\n']);
    for x = 1:t:size(Id,2)
        for y = 1:t:size(Id,1)
            fprintf(file,'v %f %f %f\n',x,1.0 - y,-1 * double(Id(y,x)));
            fprintf(file,'vt %f %f %f\n',x/size(Id,2),1.0 - y/size(Id,1),0.000);
        end
    end
    idx = 1;
    while(1)
        if(idx + y_max + 1> y_max * x_max)
            break;
        elseif(mod(idx,y_max) ~= 0)
            fprintf(file,'f %d/%d %d/%d %d/%d\n',idx,idx, idx + y_max + 1, idx + y_max + 1, idx + y_max,idx + y_max);
        end
        idx = idx + 1;
    end
    idx = 1;
    while(1)
        if(idx + y_max + 1> y_max * x_max)
            break;
        elseif(mod(idx,y_max) ~= 0)
            fprintf(file,'f %d/%d %d/%d %d/%d\n',idx,idx, idx + 1,idx + 1, idx + y_max + 1, idx + y_max + 1);
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

