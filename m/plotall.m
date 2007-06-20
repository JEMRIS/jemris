function Nimg=plotall(hax,WHAT,EPIflag,noise,img_num)

Nimg=1;
if (nargin<5);img_num=1;end
%arg 1: resolution for imaging (matrix size = res*res)
%arg 2: perform EPI reordering of every other line
colormap(gray)

%plot evolution
if WHAT==5
   [M,x,y,t]=read_selex_evol(pwd);
   Nimg=length(t);
   [Nx,Ny]=size(M(:,:,1,1));
   S=zeros(2*Nx,2*Ny);
   %S(1:Nx,1:Ny)=sqrt(sum(M(:,:,img_num,1:2).^2,4));
   %S(1:Nx,Ny+[1:Ny])=atan2(M(:,:,img_num,2),M(:,:,img_num,1))/pi;
   S(1:Nx,1:Ny)=M(:,:,img_num,1);
   S(1:Nx,Ny+[1:Ny])=M(:,:,img_num,2);
   S(Nx+[1:Nx],Ny+[1:Ny])=M(:,:,img_num,3);
   imagesc(S),h=colorbar('peer',hax{2},'southoutside');%set(h,'position',get(h,'position')-[.05 0 0 0]);
   axis image,set(gca,'xtick',[],'ytick',[])
   %text(.25*Nx,1.5*Ny,sprintf('|M_{xy}|   \\phi_{xy} \n\n           M_{z}'),'color','r','fontsize',12,'fontweight','bold')
   text(.25*Nx,1.6*Ny,sprintf('M_{x}   M_{y} \n\n        M_{z}'),'color','y','fontsize',12,'fontweight','bold')
   title(sprintf('t = %5.2f ms',t(img_num)),'fontsize',12,'fontweight','bold')
   return
end

%read signal
if WHAT~=1
 f=fopen('signal.bin'); A=fread(f,Inf,'double','l');fclose(f);
 n=size(A,1)/4; A =reshape(A,4,n)';t=A(:,1);[t,I]=sort(t);M=A(I,2:4);
 d=diff(diff(t));d(d<1e-5)=0;I=[0;find(d)+1;length(t)];
end

%add noise scaled with bandwidth (only to x- and y-component)
if noise
  f=fopen('sample.bin'); A=fread(f,Inf,'double','l');fclose(f);
  n=size(A,1)/8; A =reshape(A,8,n)'; M0=max(A(:,1));
  for i=1:length(I)-1
   J=[I(i)+1:I(i+1)];
   for i=1:2;
       M(J,i)=M(J,i)+randn(size(M(J,i))).*(M0./gradient(1e3*t(J)))*noise/100;
   end
  end
end

%plot signal
if WHAT==2
 set(gca,'visible','on')
 hold on
  for i=1:length(I)-1
   J=[I(i)+1:I(i+1)];
   plot(t(J),M(J,:),'linewidth',2)
  end
 hold off
 set(gca,'xlim',[min(t) max(t)],'box','on')
 xlabel('time [msec]','fontsize',12,'fontweight','bold')
 %ylabel('signal','fontsize',12,'fontweight','bold')
 h=legend('Mx','My','Mz'); set(h,'textcolor',[0 0 0]);
 return;
end

%plot kspace(s) / image(s)
if ( WHAT>2 ) %& mod(size(M,1),res.^2)==0 )
  S=[];
  for i=1:length(I)-1
   J=[I(i)+1:I(i+1)]';%[i length(J)]
   if (i>1 & ~isequal(size(J),size(S(:,1))) ),disp('error ... not an imaging sequence'),return,end
   S(:,i)=M(J,1)+sqrt(-1)*M(J,2);
  end
  
  %check for multiple images
  if ( size(S,2)>size(S,1) & mod(size(S,2),size(S,1)) == 0  ) 
      Nimg = size(S,2)/size(S,1);
      S=reshape(S,size(S,1),size(S,1),Nimg);
      S=S(:,:,img_num);
  end

  if EPIflag
   S(:,1:2:end)=flipud(S(:,1:2:end));
  end
  
  %figure(ii+2);clf;colormap(gray)
  %subplot(121)
  if WHAT==3
   imagesc(abs(log(abs(S')/min(abs(S(:))))))
   axis image,set(gca,'xtick',[],'ytick',[])
   title('(log) k space','fontsize',12,'fontweight','bold')
   xlabel('Readout (Freq. Enc.)','fontsize',12,'fontweight','bold')
   ylabel('Phase Encode','fontsize',12,'fontweight','bold')
  end
  if WHAT==4
   [Nx,Ny]=size(S);
   wx=.5*(1 - cos(2*pi*(1:Nx/16)'/(Nx+1))); wx = [wx; ones(Nx*7/8,1); wx(end:-1:1)];
   wy=.5*(1 - cos(2*pi*(1:Ny/16)'/(Ny+1))); wy = [wy; ones(Nx*7/8,1); wy(end:-1:1)];
   %FS=abs(fftshift(fft2(((wx*wy').*S)')));
   FS=abs(fftshift(fft2(S')));
   imagesc(fliplr(FS))
   axis image,set(gca,'xtick',[],'ytick',[])
   xlabel('Readout (Freq. Enc.)','fontsize',12,'fontweight','bold')
   ylabel('Phase Encode','fontsize',12,'fontweight','bold')
   title('image space','fontsize',12,'fontweight','bold')
  end
 
 return
end

%plot sample
if WHAT==1

 f=fopen('sample.bin'); A=fread(f,Inf,'double','l');fclose(f);
 n=size(A,1)/8; A =reshape(A,8,n)';
 d=unique(sort(A(:,6)));d=diff(d(1:2));
 XYZ=round(A(:,[6:8])/d);
 X=XYZ(:,1);X=X+abs(min(X))+1;
 Y=XYZ(:,2);Y=Y+abs(min(Y))+1;
 Z=XYZ(:,3);Z=Z+abs(min(Z))+1;
 Nx=max(X);Ny=max(Y);Nz=max(Z);
 I=sub2ind([Nx Ny Nz],X,Y,Z);
 T={'M0','T1','T2','CS'};


 for i=1:4
  B=zeros(Nx,Ny,Nz);
  if i==4;j=5;else;j=i;end
  B(I)=A(:,j);
  axes(hax{2+i})
  if Nz>1; B=B(:,:,round(Nz/2)); end
  imagesc(flipud(B'))
  axis image,axis off,title(T{i},'fontsize',12,'fontweight','bold')
  colorbar
 end
 
 return
end


