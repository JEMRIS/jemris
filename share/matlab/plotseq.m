function [SeqTree,ax]=plotseq(handles,moment_flag,kspace_flag)
%[SeqTree,ax]=plotseq(handles,moment_flag,kspace_flag)
%helper function of the JEMRIS_seq GUI - called for every redraw
%input : GUI handles and flags, if moments / k-space are drawn  
%output: Sequence tree and axes limts for the seuqence diagram

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
hax=handles.hax;
ax=handles.ax;
SeqTree=[];
zoom(gcf,'off');
set(handles.zoomFlag,'Value',0);

% 1st: draw sequence tree
if handles.plotSD==0
    set(handles.seq_dump,'Visible','on');
    set(handles.kspace_flag,'Visible','off');
    set(handles.draw_moments,'Visible','off');
    set(handles.show_interval,'Visible','off');
    set(handles.addADCs,'Visible','off');
    set(handles.ContDraw,'Visible','off');
    set(handles.text8,'Visible','off');
    set(handles.text9,'Visible','on');
    set(handles.zoomFlag,'Visible','off');
    set(handles.DrawSD,'Value',0);
    axes(hax{7}); cla(hax{7},'reset');
    SeqTree=plotSeqTree(handles.Seq,handles);
    for i=1:6;cla(hax{i},'reset');set(hax{i},'visible','off');end
    cla(hax{8},'reset');set(hax{8},'visible','off')
    return;
else
    set(handles.text9,'Visible','off');
    cla(hax{7},'reset');
    set(hax{7},'visible','off')
    set(handles.seq_dump,'Visible','off');
end

% rest of the file: draw sequence pulse diagram or k-space trajecotry
if nargin<3;kspace_flag=0;end
set(handles.kspace_flag,'Visible','on');
set(handles.zoomFlag,'Visible','on')
vis_set(handles.draw_moments ,kspace_flag==0);
vis_set(handles.show_interval,kspace_flag==0);
vis_set(handles.text8        ,kspace_flag==0);
vis_set(handles.addADCs      ,kspace_flag~=0);
vis_set(handles.ContDraw     ,kspace_flag~=0 || handles.dm ~=0);
if kspace_flag==0;cla(hax{8},'reset');set(hax{8},'visible','off');end

t  =h5read('seq.h5','/seqdiag/T');
RXP=h5read('seq.h5','/seqdiag/RXP');
TXM=h5read('seq.h5','/seqdiag/TXM');
TXP=h5read('seq.h5','/seqdiag/TXP');
GX =h5read('seq.h5','/seqdiag/GX');
GY =h5read('seq.h5','/seqdiag/GY');
GZ =h5read('seq.h5','/seqdiag/GZ');
KX =h5read('seq.h5','/seqdiag/KX');
KY =h5read('seq.h5','/seqdiag/KY');
KZ =h5read('seq.h5','/seqdiag/KZ');
B  =h5read('seq.h5','/seqdiag/META'); 
J=find(B==32); % TPOIs of excitation pulses

A=[TXM TXP GX GY GZ KX KY KZ]; A(:,2)=A(:,2)*180/pi;
%t=t(1:end-2); A=A(1:end-2,:);RXP=RXP(1:end-2);

%find all ADC events from bitmask META (jemris ADCFlag)
%   1 : ADC
%   2 : an imaging ADC (for ismrmrd output)
%   4 : auto-calibration scan (for ismrmrd output)
%   8 : phase correction scan (for ismrmrd output)
%  16 : noise scan (for ismrmrd output)
% (32 : excitation pulse)
% (64 : refocusing pulse)
for i=1:5;
 Iadc{i}=find(bitget(B,i));
 Tadc{i}=t(Iadc{i});
 Rec_Phs{i}=RXP(Iadc{i})*180/pi;
end

%shift receiver phases of multiple ADC events for better visivilty 
for i=1:5
    for j=1:5 
        if i>j
            [~,~,Ib]=intersect(Iadc{i},Iadc{j});
            Rec_Phs{i}(Ib)=Rec_Phs{i}(Ib)+10;
        end
    end
end

if length(ax)==1;ax=[min(t) max(t)];end

DO_2D = ( max(diff(KZ))<1e-8 );
[~,i1]=min(abs(t-ax(1)));
[~,i2]=min(abs(t-ax(2)));


%continuous moments
if ( handles.cd==1 && ( kspace_flag || moment_flag ) )
    for j=1:length(J)
        if j<length(J);n_end=J(j+1)-2+handles.cd;else n_end=length(A(:,3))-1;end
        n_all=[J(j)+1-handles.cd:1:n_end];            
        if j>1, for k=1:3; A(n_all,5+k)=A(n_all,5+k)+a(k); end, end
        a=A(n_end,6:8);
    end
end


%plot k-space trajecotry
if kspace_flag
        
        for i=1:6;cla(hax{i},'reset');set(hax{i},'visible','off');end
        axes(hax{8});cla(hax{8},'reset');set(gca,'visible','on');hold on
        C=flipud(autumn(length(J))); a=zeros(1,3);
        
        for j=1:length(J)
            
            if j<length(J);n_end=J(j+1)-2+handles.cd;else n_end=length(A(:,3))-1;end
            n_all=[J(j)+1-handles.cd:1:n_end];
            
            if DO_2D
                plot(A(n_all,6),A(n_all,7),'color',C(j,:))
            else
                plot3(A(n_all,6),A(n_all,7),A(n_all,8),'color',C(j,:)),view(3),grid
            end
        end
        
        %add ADCs
        if kspace_flag==2;
            if DO_2D
                plot(A(Iadc{2},6),A(Iadc{2},7),'.g')
            else
                plot3(A(Iadc{2},6),A(Iadc{2},7),A(Iadc{2},8),'.g')
            end
        end 
        
        set(gca,'color',[0 0 0]);
        if (min(A(:,6))<max(A(:,6)) && min(A(:,7))<max(A(:,7)) )
            axis(1.1*[min(A(:,6)) max(A(:,6)) min(A(:,7)) max(A(:,7))]);
        end
        axis equal; hold off;
        xlabel('Kx [rad / mm]','fontsize',12,'fontweight','bold')
        ylabel('Ky [rad / mm]','fontsize',12,'fontweight','bold')
        if ~DO_2D
            zlabel('Kz [rad / mm]','fontsize',12,'fontweight','bold')
            set(gca,'xcolor',[.5 .5 .5],'ycolor',[.5 .5 .5],'zcolor',[.5 .5 .5])
        end

        if length(J)>2
            colormap(C);colorbar('ticks',[0 1],'ticklabels',{'early','late'});%set(h,'ytick',[],'color',[0 0 0]);
        end
        return
end

%plot sequence diagram

Iax=[1 2 3 4 5];
YL={'ADC{\phi}','|RF|','RF{\phi}','GX','GY','GZ'};
if moment_flag
    Iax=[1 2 6 7 8];
end

%plot ADCs
cla(hax{1},'reset');axes(hax{1})
ADCCOLOR={'k','r','b','c','m'};
for i=1:5
 J=find(Tadc{i}>ax(1) & Tadc{i}<ax(2));
 TA=Tadc{i}(J);
 RP=Rec_Phs{i}(J);
 Irp=find(RP>180);RP(Irp)=RP(Irp)-360;
 hold on, hadc=plot(TA,RP,'.'); hold off
 save tmp hadc
 set(hadc,'color',ADCCOLOR{i});
end
set(gca,'xticklabel',[])
set(gca,'box','on')
set(gca,'xlim',[ax(1) ax(2)],'ylim',[-180 180]),grid('on')
ylabel(YL{1},'fontsize',14,'fontweight','bold')

if i1>1,i1=i1-1;end
if i2<length(t),i2=i2+1;end
I=i1:i2;

%plot remaining axes
for i=1:5
 cla(hax{i+1},'reset');axes(hax{i+1}),plot(t(I),A(I,Iax(i)),'linewidth',2),grid
 ylabel(YL{i+1},'fontsize',14,'fontweight','bold')
 set(gca,'xlim',[ax(1) ax(2)])
 ay=get(gca,'ylim');
 if abs(ay(1)-ay(2))<1e-10,set(gca,'ylim',100*ay);end
 if (i==2 || i==5),xlabel('t [msec]','fontsize',12),else set(gca,'xticklabel',[]),end
end


if (nargout==0);clear t;end
return

%%%
function vis_set(h,b)
if b
    set(h,'Visible','on');
else
    set(h,'Visible','off');
end
