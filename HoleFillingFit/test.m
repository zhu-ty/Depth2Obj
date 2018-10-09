beta0 = ones(5,1);
data2 = double(data);

xxn = data2(:,1:2);
yyn = data2(:,3);

% mdl = fitnlm(xxn, yyn,@depth_model,beta0)
%failed



%try 1/d=ax+by+c?
yyn1 = 1 ./ yyn;

r = regress(yyn1,[xxn,ones(size(xxn,1),1)])