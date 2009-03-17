function Nimg=plotsim(handles,WHAT)
%
% plotsim.m helper function of JEMRIS_sim.m
%

%
%  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
%                                  Forschungszentrum Jülich, Germany
%
%  This program is free software; you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation; either version 2 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program; if not, write to the Free Software
%  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
%
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
 t=[];M=[];I=[];
 for i=1:length(handles.rxca)
   f=fopen(sprintf('signal%02d.bin',i)); A=fread(f,Inf,'double','l');fclose(f);
   n=size(A,1)/4; A=reshape(A,4,n)';ti=A(:,1);[ti,J]=sort(ti);Mi(:,:,i)=A(J,2:4);
   d=diff(diff(ti));d(d<1e-5)=0;Ii=[0;find(d)+1;length(ti)];
   t=[t;ti];M=[M;Mi];I=[I;Ii];
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
   if size(FS,1)==1
       dt=t(2)-t(1);
       f=[0:.5/dt/(size(FS,2)/2):.5/dt];
       f=[-fliplr(f) f(2:end-1)];
       %X=fftshift(fft(S'));
       %size(f),size(X),pause
       %A=20*log(abs(X)/max(abs(X)))/log(10);
       if WHAT==5;plot(f,abs(FS),'b'),YL='amplitude [a.u.]';end
       if WHAT==6;plot(f,unwrap(angle(FS))*180/pi,'b'),YL='phase [deg]';end
       axis([-.5/dt .5/dt -inf inf])
       xlabel('frequency [kHz]','fontsize',12,'fontweight','bold')
       ylabel(YL,'fontsize',12,'fontweight','bold')
   else
       if WHAT==5;imagesc(abs(fliplr(FS)));end
       if WHAT==6;imagesc(angle(fliplr(FS)));end
       axis image,set(gca,'xtick',[],'ytick',[])
       xlabel('Readout (Freq. Enc.)','fontsize',12,'fontweight','bold')
       ylabel('Phase Encode','fontsize',12,'fontweight','bold')
       title('image space','fontsize',12,'fontweight','bold')
   end
 
 return
end

%plot sample
if WHAT==1

    f=fopen(handles.sample.file); A=fread(f,Inf,'double'); fclose(f);
    N=zeros(1,3); res=N; offset=N;
    for i=1:3
        N(i)=A(1+(i-1)*3);
        res(i)=A(2+(i-1)*3);
        offset(i)=A(1+(i-1)*3);
    end

 A=A(10:end); A=reshape(A,[5 N]); A=permute(A,[2 3 4 1]); %permutes to (X,Y,Z,type)
 I=find(A(:,:,:,1));
 Np=length(I);

 T={'M0','T1 [msec]','T2 [msec]','\Delta\omega [Hz]'};

 for i=1:4
  B=zeros(N);
  B=A(:,:,:,i);
  if (i==2 || i==3) B(I)=1./B(I); end
  axes(handles.hax{2+i})
  if N(3)>1;
      if i==1
       z=inputdlg(sprintf('enter z-position for display\n 1 <= z <= %d',N(3)),'3D-Sample',1,{num2str(round(N(3)/2))});
       z=str2num(z{1});
       if (z<1 || z >N(3)), disp('out of range ... use center'), z=round(N(3)/2); end
      end
      B=B(:,:,z); 
  end
  imagesc(flipud(B'))
  axis image,axis off,title(T{i},'fontsize',14,'fontweight','bold')
  colorbar
 end
 
 return
end


