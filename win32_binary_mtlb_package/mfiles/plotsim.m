function Nimg=plotsim(handles,WHAT)

Nimg=1;
colormap(gray)

%plot evolution
if WHAT==7
   [M,t]=readEvol(handles.sample.file,'evol',0);
   Nimg=length(t);
   [Nx,Ny,Nz]=size(M(:,:,:,1,1));
   S=zeros(2*Nx,2*Ny);
   %S(1:Nx,1:Ny)=sqrt(sum(M(:,:,img_num,1:2).^2,4));
   %S(1:Nx,Ny+[1:Ny])=atan2(M(:,:,img_num,2),M(:,:,img_num,1))/pi;img_num
   S(1:Nx,1:Ny)=M(:,:,round(Nz/2),1,handles.img_num,1);
   S(1:Nx,Ny+[1:Ny])=M(:,:,round(Nz/2),2,handles.img_num);
   S(Nx+[1:Nx],Ny+[1:Ny])=M(:,:,round(Nz/2),3,handles.img_num);
   imagesc(S),h=colorbar('peer',handles.hax{2},'southoutside');%set(h,'position',get(h,'position')-[.05 0 0 0]);
   axis image,set(gca,'xtick',[],'ytick',[])
   %text(.25*Nx,1.5*Ny,sprintf('|M_{xy}|   \\phi_{xy} \n\n           M_{z}'),'color','r','fontsize',12,'fontweight','bold')
   text(.25*Nx,1.6*Ny,sprintf('M_{x}   M_{y} \n\n        M_{z}'),'color','y','fontsize',12,'fontweight','bold')
   title(sprintf('t = %5.2f ms',t(handles.img_num)),'fontsize',12,'fontweight','bold')
   return
end

%read signal
if WHAT~=1
 for p=1:handles.Processes
  sname = 'signal';
  if handles.Processes>1,sname = sprintf('%d%s',p,sname);end
  Mp=[];if p==1,t=[];I=[];end
  for i=1:length(handles.rxca)
   f=fopen(sprintf('%s%02d.bin',sname,i)); A=fread(f,Inf,'double','l');fclose(f);
   n=size(A,1)/4; A=reshape(A,4,n)';ti=A(:,1);[ti,J]=sort(ti);Mi(:,:,i)=A(J,2:4);
   if p==1,d=diff(diff(ti));d(d<1e-5)=0;Ii=[0;find(d)+1;length(ti)];t=[t;ti];I=[I;Ii];end
   Mp=[Mp;Mi];
  end
  if p==1, M=Mp; else, M=M+Mp; end
 end
end

%add noise scaled with bandwidth (only to x- and y-component)
if handles.sim.RN && WHAT~=1
  f=fopen('sample.bin'); A=fread(f,Inf,'double');fclose(f);
  A=A(10:end); A=reshape(A,5,length(A)/5)'; M0=max(A(:,1));
  for i=1:length(I)-1
   J=[I(i)+1:I(i+1)];
   for j=1:2;
       M(J,j)=M(J,j)+randn(size(M(J,j))).*(M0./gradient(1e3*t(J)))*handles.sim.RN/100;
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
      S=S(:,:,handles.img_num);
  end

  if handles.epil
   S(:,1:2:end)=flipud(S(:,1:2:end));
  end
  
  %figure(ii+2);clf;colormap(gray)
  %subplot(121)
  if WHAT<5
   if WHAT==4,imagesc(angle(S')),end
   if WHAT==3,imagesc(abs(log(abs(S')/min(abs(S(:)))))),end
   axis image,set(gca,'xtick',[],'ytick',[])
   title('(log) k space','fontsize',12,'fontweight','bold')
   xlabel('Readout (Freq. Enc.)','fontsize',12,'fontweight','bold')
   ylabel('Phase Encode','fontsize',12,'fontweight','bold')
   return;
  end
  
   [Nx,Ny]=size(S);
   wx=.5*(1 - cos(2*pi*(1:Nx/16)'/(Nx+1))); wx = [wx; ones(Nx*7/8,1); wx(end:-1:1)];
   wy=.5*(1 - cos(2*pi*(1:Ny/16)'/(Ny+1))); wy = [wy; ones(Nx*7/8,1); wy(end:-1:1)];
   %FS=abs(fftshift(fft2(((wx*wy').*S)')));
   FS=fftshift(fft2(S'));
   if WHAT==5;imagesc(abs(fliplr(FS)));end
   if WHAT==6;imagesc(angle(fliplr(FS)));end
   axis image,set(gca,'xtick',[],'ytick',[])
   xlabel('Readout (Freq. Enc.)','fontsize',12,'fontweight','bold')
   ylabel('Phase Encode','fontsize',12,'fontweight','bold')
   title('image space','fontsize',12,'fontweight','bold')
 
 return
end

%plot sample
if WHAT==1

    A=[];N=zeros(1,3);
    for p=1:handles.Processes
     fname = handles.sample.file; 
     if handles.Processes>1
       fname = sprintf('%d%s',p,fname);
     end
     f=fopen(fname); Ap=fread(f,Inf,'double'); fclose(f);
     Np=zeros(1,3); res=Np; offset=Np;
     for i=1:3
        Np(i)=Ap(1+(i-1)*3);
        res(i)=Ap(2+(i-1)*3);
        offset(i)=Ap(3+(i-1)*3);
     end
     Ap=Ap(10:end);
     Ap=reshape(Ap,[5 Np]); Ap=permute(Ap,[2 3 4 1]); %permutes to (X,Y,Z,type)
     A=[A;Ap];
     if p==1;N=Np;else;N(1)=N(1)+Np(1);end
    end
    
 T={'M0','T1 [msec]','T2 [msec]','\Delta\omega [Hz]'};
 I=find(A(:,:,:,1));
 for i=1:4
  B=zeros(N);
  B=A(:,:,:,i);
  if (i==2 || i==3) B(I)=1./B(I); end
  axes(handles.hax{2+i})
  if N(3)>1; B=B(:,:,round(N(3)/2)); end
  imagesc(flipud(B'))
  axis image,axis off,title(T{i},'fontsize',14,'fontweight','bold')
  colorbar
 end
 
 return
end


