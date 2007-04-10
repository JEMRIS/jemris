

%constants
%N=6; n=4; dx=1.2; R=64; a=.2; Noverdet=2; tmax=20;
N=6; n=2; dx=1; R=32; a=.2; Noverdet=4; tmax=40;

%target magnetisation: n x n checkerboard with NxN submatrices
m=ones(N); M=zeros(N*n);
for i=1:n;for j=1:n
 if ( (mod(i,2) & ~mod(j,2))|(~mod(i,2) & mod(j,2)) )
  M([1:N]+(i-1)*N,[1:N]+(j-1)*N)=m;
 end
end,end
M0=zeros(n*(N+4));
M0(2*n+[1:n*N],2*n+[1:n*N])=M;
N=n*(N+4);
kmax=pi/dx;
x=dx*[-N/2+1:N/2]; y=dx*[-N/2+1:N/2];
[X,Y]=meshgrid(x,y);
X=X(:); Y=Y(:);
%k-space spiral with R turns
Nt=Noverdet*N^2; % => 2 times overdetermined inverse problem
t=[0:Nt-1]/(Nt-1);
T=t./(sqrt(a+(1-a)*t));
kx = (pi/dx)*T.*cos(T*2*pi*R);
ky = (pi/dx)*T.*sin(T*2*pi*R);

%plot results
figure(1)
 subplot(121),imagesc(x,y,M0)
 subplot(122),plot(kx,ky)
 size(M0,1)'
 pause(1)

%Fourier matrix, pseudo inverse, and B1 field solution
t=t*tmax; dt=t(2);
Gx = gradient(kx,dt);
Gy = gradient(ky,dt);
F = dt * sqrt(-1) * exp( 2 * sqrt(-1) * ( X*kx + Y*ky ) ) ;
size(F)'
P=pinv(F);
B=P*M0(:);

%plot results
figure(2)
 subplot(221),plot(t,real(B))
 subplot(222),plot(t,imag(B))
 subplot(223),plot(t,Gx)
 subplot(224),plot(t,Gy)

%write pulse shapes to binary file
%[dummy,Nt]=min(abs(t-0.3)); t=t(1:Nt); B=B(1:Nt); kx=kx(1:Nt);  ky=ky(1:Nt); 
f=fopen('2D_RfPulse.bin','wb');
 fwrite(f,Nt,'long');
 fwrite(f,[t(:) abs(B) angle(B)]','double');
fclose(f);
f=fopen('Spiral_GxPulse.bin','wb');
 fwrite(f,Nt,'long');
 fwrite(f,[t(:) Gx(:)]','double');
fclose(f);
f=fopen('Spiral_GyPulse.bin','wb');
 fwrite(f,Nt,'long');
 fwrite(f,[t(:) Gy(:)]','double');
fclose(f);

