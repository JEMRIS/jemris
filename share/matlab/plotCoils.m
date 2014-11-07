function handles=plotCoils(handles)
%draw coils
%helper function of JEMRIS_txrx.m

%
%  JEMRIS Copyright (C) 
%                        2006-2014  Tony Stoecker
%                        2007-2014  Kaveh Vahedipour
%                        2009-2014  Daniel Pflugfelder
%                                  
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

axes(handles.hax{2});
cla(handles.hax{2},'reset');
S=handles.CoilArray;
x=-1.5;y=1.5;C=[1 .7 .4];set(gca,'visible','off');
dy=.5; dx=.4; dr=.15; 
t=[0:.1:2*pi]; 
X=1.2*dr*cos(t); 
Y=1.2*dr*sin(t);
COL=[.2 .7 .7]; 

set(handles.uipanel1,'visible','off')
for ic = 1:length(S.Children)
 %draw symbol
 x = x + dx;
 a=X+x; a=[a a(1)]; b=Y+y; b=[b b(1)];
 S.Children(ic).hp=patch(a,b,COL);
 S.Children(ic).hl=line(a,b,'color',[0 0 0],'linewidth',2);
 if mod(ic,8)==0,y=y-dy;x=x-8*dx;end
 %draw 1st three chraracters of the coil name to the symbol
 ANAME='';
 for i=1:length(S.Children(ic).Attributes);
    if strcmpi(S.Children(ic).Attributes(i).Name,'NAME')
        ANAME=S.Children(ic).Attributes(i).Value;
    end
 end
 if isempty(ANAME),  ANAME=num2str(ic); end
 S.Children(ic).CoilName=ANAME;
 if length(ANAME)>3, ANAME=ANAME(1:3);  end
 xs=-0.04-0.01*length(ANAME); ys=-0.02; FS=14;
 S.Children(ic).ht=text(mean(a)+xs,mean(b)+ys,ANAME,'color',[.9 .9 .9],'fontsize',FS,'fontweight','bold');
 if (~isfield(S.Children(ic),'current')) 
     S.Children(ic).current = (ic==1);    
 end
end

    
%set axis limits
[x,y]=refine_axis(S);
x(2)=max([x(2) 1]); y(1)=min([y(1) 0]);
set(gca,'xlim',x.*[1.3 1.3],'ylim',y.*[1.1 1.1]);
%axis equal


%return Coil Array (with handles to the drawn objects)
handles.CoilArray=S;
guidata(handles.output, handles);

%button presses
set_ButtonDownFnc(handles); 
call_CurrentModule(handles);


return;


%%%%%%%%%%END OF plotSeqTree %%%%%%%%%%

%local functions

%%%% find minimal axis limits  %%%%
function [x,y]=refine_axis(S)
 x=[1 -1];y=[1 -1];
 for i=1:length(S.Children)
   XD=get(S.Children(i).hp,'XData')';
   YD=get(S.Children(i).hp,'YData')';
   x=[min([XD x(1)]) max([XD x(2)])];
   y=[min([YD y(1)]) max([YD y(2)])];
 end

%%%% set current module %%%%
function handles=set_active(hp,handles)
 for i=1:length(handles.CoilArray.Children)
    handles.CoilArray.Children(i).current = (hp==handles.CoilArray.Children(i).hp);
    %fprintf('%20s %d  %9.4f %9.4f %9.4f \n',S.Children(i).Name,S.Children(i).current,S.Children(i).hp,hp,gco);
 end

%%%% set button down function for all handles of this module %%%%
function set_ButtonDownFnc(handles)
 for i=1:length(handles.CoilArray.Children)
  C=handles.CoilArray.Children(i);
  set(C.hp,'ButtonDownFcn',{@seqcad_common,C,handles}); %pointer over patch
  set(C.hl,'ButtonDownFcn',{@seqcad_common,C,handles}); %pointer over line
  set(C.ht,'ButtonDownFcn',{@seqcad_common,C,handles}); %pointer over text
 end

%call ButtonDownFnc of the active module
function call_CurrentModule(handles)
 for i=1:length(handles.CoilArray.Children)
  %c=handles.CoilArray.Children(i); fprintf('%20s %d  %9.4f %9.4f %9.4f \n',c.Name,c.current,c.hp,gco);
  if handles.CoilArray.Children(i).current
        seqcad_common([ ], [ ], handles.CoilArray.Children(i), handles);
        return
  end
 end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% button press functions %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%all ButtonDownFnc's call this routine
function seqcad_common(src,eventdata,seq,handles) 
if isempty(handles),return,end
handles=set_active(seq.hp,handles);
guidata(handles.output, handles);
h=findobj(gca,'Type','Line');
C=get(handles.CoilMenu,'String');
if get(handles.SensMenu,'Value')==2
    plotsensitivity(handles,1);
end
for i=1:3
    if strcmpi(C{i}(1:4),seq.Name(1:4))
        set(handles.CoilMenu,'Value',i)
    end
end
for i=1:length(h), set(h,'color',[0 0 0],'linewidth',2), end
set(seq.hl,'color',[1 0 0],'linewidth',3)

%global HANDLES;
%HANDLES=handles;
%show attributes
set(handles.uipanel1,'visible','on')
T=sprintf('Coil: %s - Type: %s',seq.CoilName,seq.Name); 
if ~seq.HasMap,T=sprintf('%s   <--- !sensitvities not yet calculated!',T);end
set(handles.uipanel1,'Title',T)

%save tmp handles

A=handles.Attributes{find(strcmpi(handles.Coils,seq.Name))};
%HA=handles.HiddenAttributes{find(strcmp(handles.Modules,upper(seq.Name)))};

%return

if isempty(seq.Attributes)
    seq.Attributes.Name='Name';
    seq.Attributes.Value=seq.CoilName;
end

for i=1:14
    if i>length(A)
        bvis='''off''';
    else
        eval(['set(handles.TextAttrib',num2str(i),',''String'',''',A{i},''');'])
        s=struct2cell(seq.Attributes);
        n=find(strcmp(A{i},squeeze(s(1,:,:))));
        if isempty(n)
            val='';
            if strcmp(A{i},'Name'),val=seq.Name;end
        else
            val=seq.Attributes(n).Value;
        end
        eval(['set(handles.EditAttrib',num2str(i),',''String'',''',val,''');'])
        bvis='''on''';
    end
    eval(['set(handles.TextAttrib',num2str(i),',''Visible'',',bvis,');'])
    eval(['set(handles.EditAttrib',num2str(i),',''Visible'',',bvis,');'])
end

%special case: extended size to enter Sensitivity formula
p=get(handles.EditAttrib12,'position');
if strcmpi(get(handles.TextAttrib12,'string'),'sensitivity')
    if p(3) < 3*handles.FormulaSize; 
        p(3) = 3*p(3);
        set(handles.EditAttrib12,'position',p); 
    end
else
    p(3) = handles.FormulaSize;
    set(handles.EditAttrib12,'position',p);    
end

return

if isempty(HA)
 set(handles.HiddenAttr,'Visible','off');
else
 set(handles.HiddenAttr,'Visible','on');
 HA(2:end+1)=HA; HA{1}=' list of hidden attributes';
 for i=2:length(HA),HA{i}=strrep(HA{i},'HIDDEN','');end
 set(handles.HiddenAttr,'String',HA);
end

%change the sequence tree?
global INSERT_MODULE_NUMBER % -3 = copy, -2 = swap, -1 =delete, 1,2,... insert modules
ism=INSERT_MODULE_NUMBER;
if ism
  Seq=changeSeqTree(handles.Seq,handles,1);
  %if the tree is changed, update the handles and redraw the tree
  if ~isempty(Seq) && ism~=INSERT_MODULE_NUMBER 
    if ism==-1, Seq.current=1; end %case delete: highlight the root node
    handles.Seq=Seq;
    axes(handles.hax{7}); cla(handles.hax{7},'reset');
    handles.Seq=plotSeqTree(handles.Seq,handles);
    guidata(handles.output, handles);
  end
end

