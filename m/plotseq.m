function [SeqTree,ax]=plotseq(handles,moment_flag,kspace_flag);

%TS@IME-FZJ 03/2007
hax=handles.hax;
ax=handles.ax;

% 1st: draw sequence tree
axes(hax{7}); cla(hax{7},'reset'); SeqTree=plotSeqTree(handles.Seq,handles);

% rest of the file: draw sequence pulse diagram
if nargin<3;kspace_flag=0;end
if kspace_flag==0; cla(hax{8},'reset'); set(hax{8},'visible','off'); end

f=fopen('seq.bin'); A=fread(f,Inf,'double'); fclose(f); if A(1)==-1;return;end
A(end)=[]; n=size(A,1)/7;A=reshape(A,7,n)'; A=[0 0 0 0 0 0 -1;A]; G=A(:,[4 5]);

%ADCs in K-Space
 if kspace_flag==2
    t=A(:,1); G=cumtrapz(t,G); J=find(diff(A(:,2)));J=J(2:2:end);
    for j=1:length(J)
        fact=1; %try to guess phase inversions from 180 degree pulses!
        if abs(pi-trapz(t(J(j)-1:J(j)),A(J(j)-1:J(j),2)))<1e-4;fact=2;end
        G(J(j)+1:end,1)=G(J(j)+1:end,1)-fact*G(J(j),1);
        G(J(j)+1:end,2)=G(J(j)+1:end,2)-fact*G(J(j),2);
    end
 end
I=find(A(:,7)<0); J=find(A(:,7)>0);
Tadc=A(J,1); G=G(J,:);
t=A(I,1); A=A(I,[2:6]); A(:,2)=A(:,2)*180/pi;

if length(ax)==1;ax=[min(t) max(t)];end

[dummy,i1]=min(abs(t-ax(1)));
[dummy,i2]=min(abs(t-ax(2)));

if moment_flag
    A(:,[3 4])=cumtrapz(t,A(:,[3 4]));
    %set moment to zero after every RF pulse
    J=find(diff(A(:,1)));J=J(2:2:end);
    for j=1:length(J)
        fact=1; %try to guess phase inversions from 180 degree pulses!
        if abs(pi-trapz(t(J(j)-1:J(j)),A(J(j)-1:J(j),1)))<1e-4;fact=2;end
        A(J(j)+1:end,3)=A(J(j)+1:end,3)-fact*A(J(j),3);
        A(J(j)+1:end,4)=A(J(j)+1:end,4)-fact*A(J(j),4);
    end
    if kspace_flag
        for i=1:6;cla(hax{i},'reset');set(hax{i},'visible','off');end
        axes(hax{8});set(gca,'visible','on');hold on
        C=flipud(autumn(length(J)));
        for j=1:length(J)
            if j<length(J);n_end=J(j+1)-1;else,n_end=length(A(:,3));end
            n_all=J(j)+1:n_end;
            plot(A(n_all,3),A(n_all,4),'color',C(j,:))
        end
        if kspace_flag==2;plot(G(:,1),G(:,2),'.g');end
        set(gca,'color',[0 0 0]);
        if (min(A(:,3))<max(A(:,3)) & min(A(:,4))<max(A(:,4)) )
            axis(1.1*[min(A(:,3)) max(A(:,3)) min(A(:,4)) max(A(:,4))]);
        end
        axis equal; hold off;
        xlabel('Kx [rad / mm]','fontsize',12,'fontweight','bold')
        ylabel('Ky [rad / mm]','fontsize',12,'fontweight','bold')
        if length(J)>1
            colormap(C);h=colorbar;set(h,'ytick',[],'color',[0 0 0]);
            axes(h);xlabel('early','color',[0 0 0]);title('late','color',[0 0 0])
        end
        return
    end
end

J=find(Tadc>ax(1) & Tadc<ax(2));
Tadc=Tadc(J);

if i1>1,i1=i1-1;end
if i2<length(t),i2=i2+1;end
I=i1:i2;

YL={'ADC','|RF|','RF{\phi}','GX','GY','GZ'};
axes(hax{1}),plot(Tadc,zeros(size(Tadc)),'.r'),set(gca,'ytick',[],'xticklabel',[])
set(gca,'xlim',[ax(1) ax(2)],'ylim',[-1 1]),grid
ylabel(YL{1},'fontsize',14,'fontweight','bold')

for i=1:5
 axes(hax{i+1}),plot(t(I),A(I,i),'linewidth',2),grid
 ylabel(YL{i+1},'fontsize',14,'fontweight','bold')
 set(gca,'xlim',[ax(1) ax(2)])
 if (i==2 || i==5),xlabel('t [msec]','fontsize',12),else,set(gca,'xticklabel',[]),end
end


if (nargout==0);clear t;end
return

