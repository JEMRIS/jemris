function Nimg=plotsim(handles,WHAT)
%
% plotsim.m helper function of JEMRIS_sim.m
%

%
%  JEMRIS Copyright (C)
%                        2006-2015  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2015  Daniel Pflugfelder
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
   S(1:Nx,Ny+[1:Ny])=M(:,end:-1:1,round(Nz/2),2,handles.img_num);
   S(Nx+[1:Nx],Ny+[1:Ny])=M(:,end:-1:1,round(Nz/2),3,handles.img_num);
   imagesc(S'),h=colorbar('peer',handles.hax{2},'southoutside');%set(h,'position',get(h,'position')-[.05 0 0 0]);
   if (size(M,2) ~= 11),axis image,end
   set(gca,'xtick',[],'ytick',[])
   %text(.25*Nx,1.5*Ny,sprintf('|M_{xy}|   \\phi_{xy} \n\n           M_{z}'),'color','r','fontsize',12,'fontweight','bold')
   text(1.5*Nx,0.5*Ny,sprintf('M_{x} \n\nM_{y}    M_{z}'),'color','y','fontsize',12,'fontweight','bold')
   title(sprintf('t = %5.2f ms',t(handles.img_num)),'fontsize',12,'fontweight','bold')
   return
end

%read signal
if WHAT~=1
 channel=handles.channel;
 %save tmp handles
 if channel>0
    A = (h5read ('signals.h5', sprintf('/signal/channels/%02i',channel-1)))';
    t = h5read ('signals.h5', sprintf('/signal/times'));
    [t,J]=sort(t);M=A(J,:);
    d=diff(diff(t));d(d<1e-5)=0;I=[0;find(d)+1;length(t)];
 else
     if WHAT~=5; display('ups.'), return;end;
     % plot SumofSquares; only possible with mag images.
     FS=0; 
     for k=1:abs(channel)
       A = (h5read ('signals.h5', sprintf('/signal/channels/%02i',k-1)))';
       t = h5read ('signals.h5', sprintf('/signal/times'));
       [t,J]=sort(t); M=A(J,:);
       d=diff(diff(t));d(d<1e-5)=0;I=[0;find(d)+1;length(t)];
%copy + paste from single channel:       
      S=[];
      for ik=1:length(I)-1
       J=[I(ik)+1:I(ik+1)]';%[i length(J)]
       if (ik>1 && ~isequal(size(J),size(S(:,1))) ),disp('error ... not an imaging sequence'),return,end
       S(:,ik)=M(J,1)+sqrt(-1)*M(J,2);
      end
      %check for multiple images
      if ( size(S,2)>size(S,1) && mod(size(S,2),size(S,1)) == 0  ) 
          Nimg = size(S,2)/size(S,1);
          S=reshape(S,size(S,1),size(S,1),Nimg);
          S=S(:,:,handles.img_num);
      end
      if handles.epil
       S(:,1:2:end)=flipud(S(:,1:2:end));
      end
      FS=FS+abs(fliplr(fft2(ifftshift(S')))).^2;
%end copy+paste from single channel
     end
     imagesc(fftshift(sqrt(FS)/abs(channel)))
       axis image,set(gca,'xtick',[],'ytick',[])
       xlabel('Readout (Freq. Enc.)','fontsize',12,'fontweight','bold')
       ylabel('Phase Encode','fontsize',12,'fontweight','bold')
       title('Sum of Squares image','fontsize',12,'fontweight','bold');
    return;
 end
end

%plot signal
if WHAT==2
 set(gca,'visible','on')
 hold on
  for i=1:length(I)-1
   J=[I(i)+1:I(i+1)];
   plot(t(J),M(J,:),'linewidth',2)
   if ~verLessThan('matlab', '8.4'),set(gca, 'ColorOrderIndex', 1),end %correct line colors in new Matlab (R2015)
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
   if (i>1 && ~isequal(size(J),size(S(:,1))) ),disp('error ... not an imaging sequence'),return,end
   S(:,i)=M(J,1)+sqrt(-1)*M(J,2);
  end
  
  %check for multiple images
  if ( size(S,2)>size(S,1) && mod(size(S,2),size(S,1)) == 0  ) 
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
   FS=fliplr(fft2(ifftshift(S')));
   if size(FS,1)==1
       dt=t(2)-t(1);
       f=[0:.5/dt/(size(FS,2)/2):.5/dt];
       f=[-fliplr(f) f(2:end-1)];
       %X=fftshift(fft(S'));
       %size(f),size(X),pause
       %A=20*log(abs(X)/max(abs(X)))/log(10);
       if WHAT==5;plot(f,abs(fftshift(FS)),'b'),YL='amplitude [a.u.]';end
       if WHAT==6;plot(f,unwrap(angle(FS))*180/pi,'b'),YL='phase [deg]';end
       axis([-.5/dt .5/dt -inf inf])
       xlabel('frequency [kHz]','fontsize',12,'fontweight','bold')
       ylabel(YL,'fontsize',12,'fontweight','bold')
   else
       if WHAT==5;imagesc(fftshift(abs(FS)));end
       if WHAT==6;imagesc(fftshift(angle(FS)));end
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

 A      = h5read (handles.sample.file, '/sample/data');
 res    = h5read (handles.sample.file, '/sample/resolution');
 offset = h5read (handles.sample.file, '/sample/offset');

 N(1) = size(A,2);
 N(2) = size(A,3);
 N(3) = size(A,4);
 
A=permute(A,[2 3 4 1]); %permutes to (X,Y,Z,type)
 I=find(A(:,:,:,1));
 Np=length(I);

 T={'M0','T1 [msec]','T2 [msec]','\Delta\omega [rad/sec]'};

 for i=1:4
  B=zeros(N);
  B=A(:,:,:,i);
  if (i==2 || i==3) B(I)=1./B(I); end
  if (i==4)
      B=A(:,:,:,5);
      R2=A(:,:,:,3); R2S=A(:,:,:,4); 
      dBT2s = tan(pi*(rand(length(I),1)-.5)).*(R2S(I)-R2(I));
      a=max(R2S(I)-R2(I));
      dBT2s(abs(dBT2s)>2*a)=0;dBT2s((R2S(I)-R2(I))<0)=0;
      B(I) = B(I) + 1000*dBT2s; %convert 1/T2prime to rad/sec
  end
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
  axis off,title(T{i},'fontsize',14,'fontweight','bold')
  if (size(B,2) ~= 11),axis image,end
  colorbar
 end
 
 return
end


