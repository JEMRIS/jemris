function writeSample(Sample)
%

N=size(Sample.M0);
n=length(N);
for i=1:n
    if mod(N(i),2)
        x{i}=[1:N(i)]-(N(i)-1)/2-1;
    else
        x{i}=[1:N(i)]-N(i)/2;
    end
end

if n==2
 [X,Y]=meshgrid(Sample.dx*x{1},Sample.dy*x{2}); Z=0*X;
end

if n==3
 [X,Y,Z]=meshgrid(Sample.dx*x{1},Sample.dy*x{2},Sample.dz*x{3});
end

I=find(Sample.M0);
N=length(I);

f=fopen(Sample.fname,'w','a');
 fwrite(f,N,'double');
 for i=1:N
  fwrite(f,[Sample.M0(I(i)),Sample.T1(I(i)),Sample.T2(I(i)),0,...
         Sample.DB(I(i)),X(I(i)),Y(I(i)),Z(I(i))],'double');
 end
fclose(f);
