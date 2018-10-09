function y = depth_model(beta,x)


b1 = beta(1);
b2 = beta(2);
b3 = beta(3);
b4 = beta(4);
b5 = beta(5);

x1 = x(:,1);
x2 = x(:,2);


y = b4./(b1*x1+b2*x2+b3)+b5;

end

