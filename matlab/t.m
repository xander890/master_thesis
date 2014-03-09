clear all


f = 'abs001deg80_dir';
file=fopen(strcat(f,'.txt'));
s = textscan(file,'%s','Delimiter',' ');
t1=s{1,1};
for i=1:length(t1)
   jdata(i)= str2double(t1(i));
end

file2=fopen(strcat(f,'_alex.txt'));
s2 = textscan(file2,'%s','Delimiter','\n');
t1=s2{1,1};
for i=1:length(t1)
   adata(i)= str2double(t1(i));
end

fid = fopen(strcat(f,'_res.raw'),'rb');
A = fread(fid,[1 512], 'float');


x = linspace(-16,16,512);
x = x';
semilogy(x,jdata,'b',x,adata, 'r');
RMSE = sqrt(mean((adata - jdata).^2));
RMSE2 = sqrt(mean((A - jdata).^2));
RMSE3 = sqrt(mean((A - adata).^2));
sprintf('RMSE(Alex vs. reference): %f\nRMSE(Simulator vs. reference): %f\nRMSE(Simulator vs. Alex): %f\n', RMSE,RMSE2,RMSE3)