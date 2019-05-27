function handles=plotsensitivity(handles,plot_coils_call)
%
% plotsensitivity.m helper function of JEMRIS_txrx.m
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

if nargin<2,handles=plotCoils(handles);end
cla(handles.hax{1},'reset');
axes(handles.hax{1});
set(gca,'visible','on');
colormap(jet)

NC=length(handles.CoilArray.Children); %number of coils

%show sensitivity of all coils or just the selected one?
if get(handles.SensMenu,'Value')==1
    COILS = 1:NC; 
else
    for i=1:NC
        if handles.CoilArray.Children(i).current
            COILS=i;
            break;
        end
        COILS = 1:NC; %if no coil is selected
    end
end

%loop over coil(s)
CENT=cell(NC,1); AZIM=cell(NC,1); POL =cell(NC,1); NAME=cell(NC,1);

for i=COILS
    if ~handles.CoilArray.Children(i).HasMap,continue,end
    NA=length(handles.CoilArray.Children(i).Attributes);
    N=0; DIM=0; AxLim=[0 0];
    AZIM{i}=NaN;  POL{i}=NaN;  CENT{i}=[0 0 0];
    NAME{i}=handles.CoilArray.Children(i).CoilName;
    for j=1:NA
        if strcmpi('DIM',handles.CoilArray.Children(i).Attributes(j).Name)
            DIM   = str2double(handles.CoilArray.Children(i).Attributes(j).Value);
        end
        if strcmpi('AZIMUTH',handles.CoilArray.Children(i).Attributes(j).Name)
            AZIM{i}= str2double(handles.CoilArray.Children(i).Attributes(j).Value);
        end
        if strcmpi('POLAR',handles.CoilArray.Children(i).Attributes(j).Name)
            POL{i} = str2double(handles.CoilArray.Children(i).Attributes(j).Value);
        end
        if strcmpi('EXTENT',handles.CoilArray.Children(i).Attributes(j).Name)
            v     = str2double(handles.CoilArray.Children(i).Attributes(j).Value);
            AxLim = 0.5*[-v v]; 
        end
        if strcmpi('POINTS',handles.CoilArray.Children(i).Attributes(j).Name)
            N     = str2double(handles.CoilArray.Children(i).Attributes(j).Value);
        end
        ax={'X','Y','Z'};
        for k=1:3
         as = handles.CoilArray.Children(i).Attributes(j).Name;
         if strcmpi([ax{k},'POS'],as)
             CENT{i}(k) = str2double(handles.CoilArray.Children(i).Attributes(j).Value);
         end
        end
    end
        

end

%H.Groups.Groups(1).Datasets(1).Dataspace.Size;
A=[];
for i=1:NC
 if ~handles.CoilArray.Children(i).HasMap,continue,end
 a = h5read ('sensmaps.h5', sprintf('/maps/magnitude/%02d',i-1)); 
 p = h5read ('sensmaps.h5', sprintf('/maps/phase/%02d',i-1));
 if DIM==2
  A(:,:,i) = a.*exp(sqrt(-1)*(p));
 else
  A(:,:,:,i) = a.*exp(sqrt(-1)*(p));     
 end
end

if numel(COILS)==1
    %A=permute(A(:,:,:,COILS),[2 1 3]);
    if DIM==2 %(ndims(A)==3)
        A = A(:,:,COILS);
    else
        A = A(:,:,:,COILS);
    end
else
    if DIM==2 %(ndims(A)==3)
        A = sum(A,3);
    else
        A = permute(sum(A,4),[2 1 3]);
    end
    %A = sum(A,ndims(A));
end
%complex data selection
C=get(handles.ComplexMenu,'String');
T=C{get(handles.ComplexMenu,'Value')};
switch T
    case 'Magnitude'
        A=abs(A);
    case 'Phase'
        A=angle(A)*180/pi;
    case 'Real'
        A=real(A);
    case 'Imaginary'
        A=imag(A);
    otherwise
        disp('???')
end

%viewing selection
C=get(handles.ViewMenu,'String');
T=C{get(handles.ViewMenu,'Value')};
NS = min([N ceil(N*get(handles.slicepos,'Value')+eps)]);
if DIM==2,NS=1;end
ax = [AxLim(1):diff(AxLim)/(N-1):AxLim(2)];
ay = ax;
az = AxLim(1)+(NS-.5)*diff(AxLim)/(N-1);
switch T
  case 'x-y slice'
        S  = A(:,:,NS);
        XL = 'X'; YL='Y'; ZL='Z';
        ix=1;iy=2;iz=0;
    case 'x-z slice'
        S  = squeeze(A(:,NS,:));
        XL = 'X'; YL='Z'; ZL='Y';
        ix=1;iy=3;iz=0;
    case 'y-z slice'
        S  = squeeze(A(NS,:,:));
        XL = 'Y'; YL='Z'; ZL='X';
       ix=2;iy=3;iz=0;
     case '3D'
        if length(size(A))<3
            set(gca,'visible','off');
            disp(['Selection ',T,' is not possible. No such data.'])
            return
        end
        az = ax;
        A=(A-min(A(:)))/(max(A(:))-min(A(:)));
        scale=get(handles.slicepos,'Value');
        if scale==1,scale=0.999;end
        p=patch(isosurface(ax,ay,az,A,scale));
        isonormals(ax,ay,az,A,p);
        set(p,'facecolor','red','edgecolor','none');
        daspect([1 1 1]);
        view(3),axis tight,grid on;
        %h=rotate3d(gcf);setAllowAxesRotate(h,handles.hax{2},false);
        camlight; lighting phong;
        alpha(.5);
        set(handles.slicepos,'sliderstep',[.01 .1])
        s{1}='isosurface';s{2}='intensity';
        set(handles.text10,'string',s)
        title(['sensitivity at ',num2str(round(100*scale)),' %'],'fontweight','bold')
        xlabel('X [mm]'),ylabel('Y [mm]'),zlabel('Z [mm]')
        axis([ax(1) ax(end) ay(1) ay(end) az(1) az(end)])
         ix=1;iy=2;iz=3;
         addCoils(ix,iy,iz,NC,CENT,AZIM,POL,NAME,handles,[0 0 0]);
         addSample(1,0,az,handles);
       return
    otherwise
        disp('unkown slice view')
end

if min(size(S))<2
    set(gca,'visible','off');
    disp(['slice ',T,' is not possible. No such data.'])
    return
end


pcolor(ay,ax,S'),shading interp,axis equal
colormap(jet(50)),colorbar
set(handles.hax{1},'color',[0 0 0])
xlabel([XL,' [mm]']),ylabel([YL,' [mm]'])
if DIM==2 || isnan(az) || isinf(az)
    set(handles.slicepos,'visible','off')    
    set(handles.text10,'visible','off')    
else
    Nz=N;
    set(handles.slicepos,'sliderstep',[1/Nz 5/Nz])
    s{1}='slice';s{2}='position';
    set(handles.text10,'string',s)    
    set(handles.slicepos,'visible','on')    
    set(handles.text10,'visible','on')    
    title(['slice ',ZL,' = ',num2str(az),' mm'])
end
addSample(max(S(:)),min(S(:)),az,handles);
addCoils(ix,iy,iz,COILS,CENT,AZIM,POL,NAME,handles,[.5 .5 0]);

%%%
function addCoils(ix,iy,iz,COILS,C,AZ,PL,T,handles,TCOL)
if get(handles.HideCoilMenu,'Value') == 2, return, end
hold on
a=get(gca,'xlim'); b=get(gca,'ylim');
MinX = a(1); MaxX=a(2); MinY = b(1); MaxY=b(2);
for i=COILS
    if ~handles.CoilArray.Children(i).HasMap,continue,end
    if iz,z=C{i}(iz);else z=0;end
    x=C{i}(ix); y=C{i}(iy);
    u=[0 0 0 0]; v=u; w=u;
    if i==1 || length(COILS)==1; d=0.12*diff(get(handles.hax{1},'xlim')); end
    if ~isnan(AZ{i})
        a=[0 0 -1 0]; b=[-.25 .25 0 -.25]; 
        c=cos(AZ{i}*pi/180); s=sin(AZ{i}*pi/180);
        for j=1:4; u(j)=c*a(j)-s*b(j); v(j)=s*a(j)+c*b(j);end
        patch(x+d*u,y+d*v,z+d*w,[1 1 0])
        %line(x+d*u,y+d*v,z+d*w,'color',[1 0 0],'linewidth',2)
    end
    %[T{i},' ',num2str(u)]
    s=.5*abs(sign(round(u(3)))-1);
    h=text(x+d*(u(3)-s),y+1.5*d*v(3),z,T{i},'color',TCOL,'fontsize',12,'fontweight','bold');
    %if ( handles.CoilArray.Children(i).current ),set(h,'fontsize',14,'color',[.7 0 0]);end
    MaxX = max([x MaxX]);  MinX = min([x MinX]);
    MaxY = max([y MaxY]);  MinY = min([y MinY]);
end
%[MinX MaxX],[MinY MaxY]
set(gca,'xlim',1.1*[MinX MaxX],'ylim',1.1*[MinY MaxY])
hold off
 
%%%
function addSample(Max,Min,az,handles)
if get(handles.HideSampleMenu,'Value') == 2, return, end

handles.sample.file = 'sample.h5';
 A      = h5read (handles.sample.file, '/sample/data');
 res    = h5read (handles.sample.file, '/sample/resolution');
 offset = h5read (handles.sample.file, '/sample/offset');
 N=zeros(1,3);
 
 N(1) = size(A,2);
 N(2) = size(A,3);
 N(3) = size(A,4);
 
 A=permute(A,[2 3 4 1]); 
 A=A(:,:,:,1); A(find(A))=Max+(Max-Min)/49; A(find(A==0))=NaN;
 x=([0:N(1)-1]-N(1)/2)*res(1);%+offset(i);
 y=([0:N(2)-1]-N(2)/2)*res(2);%+offset(i);
 z=([0:N(3)-1]-N(3)/2)*res(3);%+offset(i);
 C=get(handles.ViewMenu,'String');
 T=C{get(handles.ViewMenu,'Value')};
 NS = ceil(N*get(handles.slicepos,'Value')+eps);
 switch T
     case 'x-y slice'
         [dummy,i]=min(abs(az-z));
         if length(z) > 1 && dummy>res(1); return; end
         S=A(:,:,i)';
         if min(size(S))<2
             disp('sample has no data along these dimensions')
             set(handles.HideSampleMenu,'Value',2);
             return
         end
         hold on
         h=pcolor(x,y,S);shading interp,axis equal
         hold off
         set(h,'facealpha',.2);
     case 'x-z slice'
         [dummy,i]=min(abs(az-y));
         if length(y) > 1 && dummy>res(2); return; end
         S=squeeze(A(:,i,:))';
         if min(size(S))<2
             disp('sample has no data along these dimensions')
             set(handles.HideSampleMenu,'Value',2);
             return
         end
         hold on
         h=pcolor(x,z,S);shading flat,axis equal
         hold off
         set(h,'facealpha',.2);
     case 'y-z slice'
         [dummy,i]=min(abs(az-x));
         if length(x) > 1 && dummy>res(2); return; end
         S=squeeze(A(i,:,:))';
         if min(size(S))<2
             disp('sample has no data along these dimensions')
             set(handles.HideSampleMenu,'Value',2);
             return
         end
         hold on
         h=pcolor(y,z,S);shading flat,axis equal
         hold off
         set(h,'facealpha',.2);
     case '3D'
        %size(x),size(y),size(z),size(A)
        A(find(isnan(A)))=0;
        p=patch(isosurface(y,x,z,A,1));
        isonormals(y,x,z,A,p);
        set(p,'facecolor','blue','edgecolor','none','facealpha',.5);         
        return
     otherwise
        disp('unkown slice view')
        return
 end
 if find(~isnan(A))
    C=jet(50);
    C(end,:)=[0 0 0];
    colormap(C);
 end
        
        

 
 
 
