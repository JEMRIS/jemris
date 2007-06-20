function writeSample(Sample)
%

[Nx,Ny,Nz]=size(Sample.M0);
x=[1:Nx]-Nx/2; y=[1:Ny]-Ny/2; z=[1:Nz]-Nz/2;
[X,Y,Z]=meshgrid(Sample.dx*x,Sample.dy*y,Sample.dz*z);

I=find(Sample.M0);
N=length(I);

f=fopen(Sample.fname,'w','a');
 fwrite(f,N,'double');
 for i=1:N
  fwrite(f,[Sample.M0(I(i)),Sample.T1(I(i)),Sample.T2(I(i)),0,...
         Sample.DB(I(i)),X(I(i)),Y(I(i)),Z(I(i))],'double');
 end
fclose(f);
