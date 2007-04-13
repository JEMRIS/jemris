function [S,x]=plotSeqTree(S,handles,x,y)
%recursevely draw the sequence tree

%TS@IME 03/2007

dy=.5; dx=.4; dr=.15; t=[0:.1:2*pi]; X=1.2*dr*cos(t); Y=1.2*dr*sin(t);

if nargin<3 %the root node
  if nargin==1,handles=[];end
  x=-1.5;y=1.5;C=[1 .5 .2];set(gca,'visible','off','xlim',[-.1 .1],'ylim',[-.1 .1]);
else
 switch S.Name
    case 'ConcatSequence'
        C=[1 1 .4]; 
    case 'Parameter'
     return;    
    case 'AtomicSequence'
        C=[.4 .4 1];
    case'DelayAtom'
        C=[.4 1 .4];
    otherwise
        C=[1 1 1]; X=[-dr -dr dr dr]; Y=[-dr dr dr -dr];
 end
end

%draw symbol
a=X+x;a=[a a(1)];b=Y+y;b=[b b(1)];
S.hp=patch(a,b,C);
S.hl=line(a,b,'color',[0 0 0],'linewidth',2);
%overlay icon on pulseshapes
if isempty([findstr('Atom',S.Name) findstr('Sequence',S.Name)])
    eval(['B=handles.icons.',S.Name,';']);
    B(find(isnan(B)))=1;
    S.hi=image('xdata',mean(a)+.6*dr*[-7:8]/8,'ydata',mean(b)+.6*dr*[8:-1:-7]/8,'cdata',B);
else
    S.hi=0;
end

%call all children
oldx=x;
N=length(S.Children);
for i=1:N
    S.Children(i).hp=0; S.Children(i).hl=0;  S.Children(i).hi=0; 
    %the parameter tag uis not shown but belongs to the root node
    if strcmp(S.Children(i).Name,'Parameter');continue;end
    %pulseshape tags are drawn at different positions ...
    if isempty([findstr('Atom',S.Children(i).Name) findstr('Sequence',S.Children(i).Name)])
      S.Children(i)=plotSeqTree(S.Children(i),handles,x,y-i*(dy-.2));
    % ... than sequence tags (Concat, Atomic, Delay, ...)
    else
      [S.Children(i),x]=plotSeqTree(S.Children(i),handles,x,y-dy);
      h=S.Children(i).hp;
      %draw tree connection lines
      line([mean(a) mean(get(h,'XData'))],...
           [min(b) max(get(h,'YData'))],'color',[0 0 0],'linewidth',2)
    end
end

%show loop conection numbers
A=S.Attributes;
for j=1:length(A)
    if strcmp(A(j).Name,'ConnectToLoop')
        text(mean(a)+.07,mean(b)-.07,A(j).Value,...
             'color',[1 0 0],'fontweight','bold');
    end
end

%increase x-position
if (x==oldx),x=x+dx;end

%finally, the root: 1.) sets minimal axis limits, 2.) updates the guidata, 
%                   3.) sets the ButtonDownFncs,  4.) calls the active module
if nargin<3
    x=[1 -1];y=[1 -1];
    [x,y]=refine_axis(S,x,y);
    x(2)=max([x(2) 1]); y(1)=min([y(1) 0]);
    set(gca,'xlim',x+[-.1 .1],'ylim',y+[-.1 .1]);
    handles.Seq=S;
    guidata(handles.output, handles);
    S.Name='RootSequence';       
    set_ButtonDownFnc(S,handles) % a different Function for the root node ConcaTsequence !
    call_CurrentModule(S,handles);
    S.Name='ConcatSequence';     
    guidata(handles.output, handles);
end

%%%%%%%%%%END OF plotSeqTree %%%%%%%%%%

%local functions

%%%% find minimal axis limist recursively %%%%
function [x,y]=refine_axis(S,x,y)
 if S.hp==0;return;end
 x=[min([get(S.hp,'XData')' x(1)]) max([get(S.hp,'XData')' x(2)])];
 y=[min([get(S.hp,'YData')' y(1)]) max([get(S.hp,'YData')' y(2)])];
 for i=1:length(S.Children)
     [x,y]=refine_axis(S.Children(i),x,y);
 end

%%%% set current module recursively %%%%
function S=set_active(hp,S)
 S.current = (hp==S.hp);
 %if S.current,fprintf('%20s %d  %9.4f %9.4f %9.4f \n',S.Name,S.current,S.hp,hp,gco); end
 for i=1:length(S.Children)
     S.Children(i)=set_active(hp,S.Children(i));
 end

 %%%% set button down function for the patches and images of all modules %%%%
 function set_ButtonDownFnc(S,handles)
 eval(['set(S.hp,''ButtonDownFcn'',{@seqcad_',S.Name,',S,handles});']);
 if S.hi
     eval(['set(S.hi,''ButtonDownFcn'',{@seqcad_',S.Name,',S,handles});']);
 end
 for i=1:length(S.Children)
     set_ButtonDownFnc(S.Children(i),handles);
 end

%call ButtonDownFnc of the active module
function call_CurrentModule(S,handles);
 if S.current
        eval(['seqcad_',S.Name,'([],[],S,handles);']);
        return
 end
 for i=1:length(S.Children)
     call_CurrentModule(S.Children(i),handles);
 end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% button press functions %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%all ButtonDownFnc's call this routine
function seqcad_common(seq,handles) 

handles.Seq=set_active(seq.hp,handles.Seq);
guidata(handles.output, handles);
h=findobj(gca,'Type','Line');
for i=1:length(h), set(h,'color',[0 0 0],'linewidth',2), end
set(seq.hl,'color',[1 0 0],'linewidth',3)

%show attributes
set(handles.SeqObjectPanel,'Title',['Module: ',seq.Name])
for i=1:14
    if i>length(seq.A)
        bvis='''off''';
    else
        eval(['set(handles.SOtag',num2str(i),',''String'',''',seq.A{i},''');'])
        s=struct2cell(seq.Attributes);
        n=find(strcmp(seq.A{i},squeeze(s(1,:,:))));
        if isempty(n)
            val='';
            if strcmp(seq.A{i},'Name'),val=seq.Name;end
        else
            val=seq.Attributes(n).Value;
        end
        eval(['set(handles.SOEtag',num2str(i),',''String'',''',val,''');'])
        bvis='''on''';
    end
    eval(['set(handles.SOtag',num2str(i),',''Visible'',',bvis,');'])
    eval(['set(handles.SOEtag',num2str(i),',''Visible'',',bvis,');'])
end

%change the sequence tree?
global INSERT_MODULE_NUMBER % -2 = swap, -1 =delete, 1,2,... insert modules
ism=INSERT_MODULE_NUMBER;
if ism
  A=struct;
  if ism>0
      eval(['A=seqcad_',handles.Modules{ism},'([],[],A,[]);']);
  end
  Seq=changeSeqTree(handles.Seq,handles,A,1);
  %if the tree is changed, update the handles and redraw the tree
  if ~isempty(Seq) && ism~=INSERT_MODULE_NUMBER 
    if ism==-1, Seq.current=1; end %case delete: highlight the root node
    handles.Seq=Seq;
    axes(handles.hax{7}); cla(handles.hax{7},'reset');
    handles.Seq=plotSeqTree(handles.Seq,handles);
    guidata(handles.output, handles);
  end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function seqcad_RootSequence(src,eventdata,seq,handles)
name=seq.Name;
seq.Name=['Sequence Root'];
attr=seq.Attributes;
N=length(seq.Children(1).Attributes);
seq.Attributes(end+1:end+N)=seq.Children(1).Attributes;
seq.A={'Name','Repetitions','Factor','TR','TE','TI','TD','Nx','Ny','Nz','FOVx','FOVy','FOVz','ReadBW'};
seqcad_common(seq,handles)
seq.Name=name;
seq.Attributes=attr;

function A=seqcad_AtomicSequence(src,eventdata,seq,handles)
A={'Name'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_ConcatSequence(src,eventdata,seq,handles)
A={'Name','Repetitions','Factor','ConnectToLoop'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_DelayAtom(src,eventdata,seq,handles)
A={'Name','Delay','DelayType','StartSeq','StopSeq','Factor','ADCs'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_EmptyPulse(src,eventdata,seq,handles)
A={'Name','Duration','ADCs'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_HardRfPulseShape(src,eventdata,seq,handles)
A={'Name','FlipAngle','Phase','Duration','ADCs'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_SincRfPulseShape(src,eventdata,seq,handles)
A={'Name','FlipAngle','Phase','Bandwidth','Zeros','Factor','ADCs','ConnectToLoop'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_RfPhaseCycling(src,eventdata,seq,handles)
A={'Name','Duration','ConnectToLoop','Cycle','Phase1','Phase2','Phase3','Phase4','ADCs'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_RfSpoiling(src,eventdata,seq,handles)
A={'Name','Duration','ConnectToLoop','QuadPhaseInc','StartCycle'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_TGPS(src,eventdata,seq,handles)
A={'Name','Axis','Area','Factor','Duration','Gmax','SlewRate'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_SS_TGPS(src,eventdata,seq,handles)
A={'Name','Axis','SliceThickness','Gmax','SlewRate'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_PE_TGPS(src,eventdata,seq,handles)
A={'Name','Axis','ConnectToLoop','Area','Duration','Factor','Gmax','SlewRate'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

function A=seqcad_RO_TGPS(src,eventdata,seq,handles)
A={'Name','Axis','Area','FlatTop','Factor','Gmax','SlewRate'};
seq.A=A;
if ~isempty(handles),seqcad_common(seq,handles);end

