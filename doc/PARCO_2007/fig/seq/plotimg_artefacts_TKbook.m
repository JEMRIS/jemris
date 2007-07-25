function plotimg
%arg 1: resolution for imaging (matrix size = res*res)
%arg 2: perform EPI reordering of every other line
DIR={'EPI_Tra_ChemShift','EPI_NonLinGrad','TrueFisp_128_tra_Susc','SE_long180pulse'};
RES=[64 64 128 128]; FLIP=[1 1 0 0]; FAC=[1 1 1 .7];
CWD=pwd;
set(gcf,'paperunits','centimeters','paperposition',[0 0 20 6],'inverthardcopy','on')
colormap(gray)
TITLE={'a)','b)','c)','d)'};
for img=1:4
    
 %clf
 cd(DIR{img})    
 subplot(1,4,img)
 res=RES(img); 
 flip=FLIP(img);
 fac=FAC(img);
 f=fopen('signal.bin'); A=fread(f,Inf,'double');fclose(f);
 n=size(A,1)/4; A =reshape(A,4,n)';t=A(:,1);[t,I]=sort(t);M=A(I,2:4);
 d=diff(diff(t));d(d<1e-5)=0;I=[0;find(d)+1;length(t)-1];


 if mod(size(M,1),res.^2)==0

 for ii=1%:size(M,1)/res.^2
  I=[1:res.^2]+(ii-1)*res.^2;
  S=M(I,1)+sqrt(-1)*M(I,2); S=reshape(S,res,res);
  %EPI reordering
  if flip
   S(:,1:2:end)=flipud(S(:,1:2:end));S=flipud(S);
  end
   [Nx,Ny]=size(S);
   wx=.5*(1 - cos(2*pi*(1:Nx/16)'/(Nx+1))); wx = [wx; ones(Nx*7/8,1); wx(end:-1:1)];
   wy=.5*(1 - cos(2*pi*(1:Ny/16)'/(Ny+1))); wy = [wy; ones(Nx*7/8,1); wy(end:-1:1)];
   FS=abs(fftshift(fft2(((wx*wy').*S)')));
   %FS=abs(fftshift(fft2(S')));
   imagesc(FS.^fac), set(gca,'color','none','xtick',[],'ytick',[]),  axis image
   title(TITLE{img},'fontsize',14)
 end
 p=get(gca,'position');set(gca,'position',p+[-0.05 -.05 .02 .02]);

end
pause(1)
cd(CWD)
end
%print('-deps','../simartex.eps') 
