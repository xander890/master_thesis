file=fopen('abs01deg00_dir.txt');
s = textscan(file,'%s','Delimiter',' ');
t1=s{1,1};
for i=1:length(t1)
   jdata(i)= str2double(t1(i));
end

file2=fopen('abs01deg00_dir_alex.txt');
s2 = textscan(file2,'%s','Delimiter','\n');
t1=s2{1,1};
for i=1:length(t1)
   adata(i)= str2double(t1(i));
end

x = linspace(-16,16,512);
semilogy(x,jdata,x,adata);