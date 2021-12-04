function Nimg=plotsim(WHAT,hax,sample_file,img_num,channel)
%
% plotsim.m helper function of JEMRIS_sim.m
%

%
%  JEMRIS Copyright (C)
%                        2006-2019  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2019  Daniel Pflugfelder
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

%plot evolution (only possible in "showRight")
if WHAT==7
   [M,t]=readEvol(sample_file,'evol',0);
   Nimg=length(t);
   [Nx,Ny,Nz]=size(M(:,:,:,1,1));
   S=zeros(2*Nx,2*Ny);
   %S(1:Nx,1:Ny)=sqrt(sum(M(:,:,img_num,1:2).^2,4));
   %S(1:Nx,Ny+[1:Ny])=atan2(M(:,:,img_num,2),M(:,:,img_num,1))/pi;img_num
   S(1:Nx,1:Ny)=M(:,:,round(Nz/2),1,img_num,1);
   S(1:Nx,Ny+[1:Ny])=M(:,end:-1:1,round(Nz/2),2,img_num);
   S(Nx+[1:Nx],Ny+[1:Ny])=M(:,end:-1:1,round(Nz/2),3,img_num);
   imagesc(S'),h=colorbar('peer',hax{2},'southoutside');%set(h,'position',get(h,'position')-[.05 0 0 0]);
   if (size(M,2) ~= 11),axis image,end
   set(gca,'xtick',[],'ytick',[])
   %text(.25*Nx,1.5*Ny,sprintf('|M_{xy}|   \\phi_{xy} \n\n           M_{z}'),'color','r','fontsize',12,'fontweight','bold')
   text(1.5*Nx,0.5*Ny,sprintf('M_{x} \n\nM_{y}    M_{z}'),'color','y','fontsize',12,'fontweight','bold')
   title(sprintf('t = %5.2f ms',t(img_num)),'fontsize',12,'fontweight','bold')
   return
end

%read signal
if WHAT~=1
 info=h5info('signals.h5');
 NCH=numel(info.Groups(1).Groups(1).Datasets); % number of Rx Channels
 CCH=channel;                                  % current selected channel
 A = (h5read ('signals.h5', sprintf('/signal/channels/%02i',abs(CCH)-1)))';
 t = h5read ('signals.h5', sprintf('/signal/times'));
 [t,J]=sort(t);M=A(J,:);
 d=diff(diff(t));d(d<1e-5)=0;I=[0;find(d)+1;length(t)];
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


%plot kspace (mag / phase )
if WHAT==3 || WHAT==4 

  [KX,KY,DATA,Nimg]=read_imsrmrd(NCH,CCH,img_num);
  if isempty(KX)
        imagesc(zeros(10))
        title('could not read ismrmrd signal','fontsize',12,'fontweight','bold')
        axis image,set(gca,'xtick',[],'ytick',[],'color',[0 0 0])
        return
  end
  
   if WHAT==4
       scatter(KX, KY, 50, angle(DATA),'filled')
       title('angle(k-space)','fontsize',12,'fontweight','bold'),colorbar
   end
   if WHAT==3
       scatter(KX, KY, 50, log(abs(DATA)),'filled'),colorbar
       title('log |k-space|','fontsize',12,'fontweight','bold')
   end
   axis image,set(gca,'xtick',[],'ytick',[],'color',[0 0 0])
   xlabel('k_x','fontsize',12,'fontweight','bold')
   ylabel('k_y','fontsize',12,'fontweight','bold')
   return;
  end
  
  %plot reconstructed image, if exisiting (5=magn / 6=phaase)
  if WHAT==5 || WHAT==6; 
    if exist('gui-reco.h5','file')
      if NCH>1 && CCH < 0, NCH=1; CCH=1; end %special case: combined image was selected => plot parallel imaging reco
      IMG=h5read('gui-reco.h5',sprintf('/images/images_%1d/data',WHAT-4+2*(NCH>1)));
      IMG=squeeze(IMG(:,:,CCH,1,:));
      if WHAT==5            
            TTL='magntiude image';
      else
            TTL='phase image';
      end
      if NCH==1 && numel(info.Groups(1).Groups(1).Datasets) > 1
          TTL = [TTL,'  (bart PI reco)'];
      else
          TTL = [TTL,' (bart nufft reco)'];
      end
      Nimg=size(IMG,3);
      imagesc(flipud(IMG(:,:,img_num)')); colorbar
      title(TTL,'fontsize',12,'fontweight','bold')
      axis image,set(gca,'xtick',[],'ytick',[],'color',[0 0 0])
      xlabel('x','fontsize',12,'fontweight','bold')
      ylabel('y','fontsize',12,'fontweight','bold')
    else %plot 1D-FT of signal
       S=M(:,1)+sqrt(-1)*M(:,2);
       dt=t(2)-t(1);
       f=[0:.5/dt/(numel(S)/2):.5/dt];
       f=[-fliplr(f) f(2:end-1)];
       if WHAT==5;plot(f,abs(fftshift(S)),'b'),YL='amplitude [a.u.]';end
       if WHAT==6;plot(f,unwrap(angle(S))*180/pi,'b'),YL='phase [deg]';end
       axis([-.5/dt .5/dt -inf inf])
       title('1D-FFT (no bart reco found)','fontsize',12,'fontweight','bold')
       xlabel('frequency [kHz]','fontsize',12,'fontweight','bold')
       ylabel(YL,'fontsize',12,'fontweight','bold')
    end
  end

%plot sample (only possible in "ShowLeft")
if WHAT==1

 A      = h5read (sample_file, '/sample/data');
 res    = h5read (sample_file, '/sample/resolution');
 offset = h5read (sample_file, '/sample/offset');

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
  axes(hax{2+i})
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

return

%read 

function [KX,KY,DATA, Nimg]=read_imsrmrd(NCH,CCH,img_num)
  file = 'signals_ismrmrd.h5'; 
  %check if fle exists
  if ~exist(file,'file')
      warning('file signals_ismrmrd.h5 not avaiable')
      KX=[]; KY=[]; DATA=[]; Nimg=1;
      return
  end
  %check if ismrmrd is available
  if isempty(strfind(path,'ismrmrd'))
      if exist('/usr/local/share/ismrmrd/matlab','dir')
          addpath('/usr/local/share/ismrmrd/matlab')
          disp('ismrmrd added to Matlab path')
      else
        warning('ismrmrd matlab reader not avaiable')
        KX=[]; KY=[]; DATA=[]; Nimg=1;
        return
      end
  end
  
  dset = ismrmrd.Dataset(file);
  nacq = dset.getNumberOfAcquisitions();
  acq = dset.readAcquisition(NCH+1);
  NRO=size(acq.data{1},1);
  %run over acquisitions to find loop dimension
  dim=1; sld=0; cnd=0; sed=0; 
  for ctr = NCH+1:nacq
    acq = dset.readAcquisition(ctr);
    flags = [acq.head.flagIsSet('ACQ_IS_DUMMYSCAN_DATA')];
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_PARALLEL_CALIBRATION');
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_PHASECORR_DATA');
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_NOISE_MEASUREMENT');
    flags(end+1) = acq.head.flagIsSet('ACQ_USER1');
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA');
    if not(any(flags))
        sld = max([sld acq.head.idx.slice]);
        cnd = max([sld acq.head.idx.contrast]);
        sed = max([sld acq.head.idx.set]);
    end
  end
  dim=dim+sld+cnd+sed; %combined ismrmrd loop dimensions: slice,contrast, set
  DATA=zeros(NRO*(nacq-NCH)/dim,NCH,dim);
  KX = zeros(NRO*(nacq-NCH)/dim,1);
  KY = zeros(NRO*(nacq-NCH)/dim,1);
  
  %read data        
  for ctr = NCH+1:nacq
    acq = dset.readAcquisition(ctr);
    flags = [acq.head.flagIsSet('ACQ_IS_DUMMYSCAN_DATA')];
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_PARALLEL_CALIBRATION');
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_PHASECORR_DATA');
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_NOISE_MEASUREMENT');
    flags(end+1) = acq.head.flagIsSet('ACQ_USER1');
    flags(end+1) = acq.head.flagIsSet('ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA');
    if not(any(flags))
        data = acq.data{1};
        traj = acq.traj{1};
        slc = acq.head.idx.slice;
        cnc = acq.head.idx.contrast;
        sec = acq.head.idx.set;
        id=1+slc+cnc*(sld+1)+sec*(sld+1)*(cnd+1);
        shift = double((ctr-NCH-1)-(id-1)*(nacq-NCH-1)/dim)*NRO;
        %fprintf('%d , %d\n',id,shift)
        DATA([1:NRO]+shift,:,id)=data;
        KX([1:NRO]+shift)=traj(1,:)';
        KY([1:NRO]+shift)=traj(2,:)';
    end
  end
  
   %reduce data to the selected image and channel number
   Nimg=size(DATA,3);
   %save tmp KX KY DATA CCH Nimg img_num
   if CCH<0,CCH=1;end
   DATA=DATA(:,CCH,img_num);
 
return
