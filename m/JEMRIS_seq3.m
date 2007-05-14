function varargout = JEMRIS_seq3(varargin)
%GUI for jemris sequence visualisation

%TS@IME-FZJ 03/2007

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @JEMRIS_seq3_OpeningFcn, ...
                   'gui_OutputFcn',  @JEMRIS_seq3_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before JEMRIS_seq3 is made visible.
function JEMRIS_seq3_OpeningFcn(hObject, eventdata, handles, varargin)
colordef white

% Choose default command line output for JEMRIS_seq3
handles.output = hObject;
handles.seqfile = '';
handles.Seq=[];       % the sequence structure
handles.SeqBackup=[]; % a backup
handles.SO=struct('Attribute',cell(10,1),'Value',cell(10,1)); %sequence object properties
hax{1}=handles.axes0; hax{2}=handles.axes1; hax{3}=handles.axes2; hax{4}=handles.axes3;
hax{5}=handles.axes4; hax{6}=handles.axes5; hax{7}=handles.axes6; hax{8}=handles.axes7;
for i=1:length(hax);set(hax{i},'color',[1 1 1],'visible','off');end
handles.hax=hax;
handles.dm=0;
handles.ax=0;
handles.CWD=pwd;
handles.JemrisPath=fileparts(which('JEMRIS_seq3'));

%all modules + attributes which are currently supported by JEMRIS
handles.Modules={'ConcatSequence','AtomicSequence','DelayAtom','EmptyPulse',...
                 'HardRfPulseShape','SincRfPulseShape','TGPS','SS_TGPS','RO_TGPS','PE_TGPS',...
                 'GradientSpiral','RfPhaseCycling','RfSpoiling','RfReceiverPhase','ExternalPulseShape'};
handles.Attributes={...
        {'Name','Repetitions','Factor','ConnectToLoop'};...
        {'Name'};...
        {'Name','Delay','DelayType','StartSeq','StopSeq','Factor','ADCs'};...
        {'Name','Duration','ADCs'};...
        {'Name','FlipAngle','Phase','Duration','ADCs'};...
        {'Name','FlipAngle','Phase','Bandwidth','Zeros','Factor','ADCs','ConnectToLoop','Gap','SliceOrder'};...
        {'Name','Axis','Area','Factor','Duration','Gmax','SlewRate'};...
        {'Name','Axis','SliceThickness','Gmax','SlewRate'};...
        {'Name','Axis','Area','FlatTop','Factor','Gmax','SlewRate'};...
        {'Name','Axis','ConnectToLoop','Area','Duration','Factor','Gmax','SlewRate'};...
        {'Name','Axis','Duration','Turns','Parameter','Resolution'};...
        {'Name','Duration','ConnectToLoop','Cycle','Phase1','Phase2','Phase3','Phase4','ADCs'};...
        {'Name','Duration','ConnectToLoop','QuadPhaseInc','StartCycle'};...
        {'Name','Phase'};...
        {'Name','FileName','Axis','Factor'}...
    } ;
handles.RootAttributes={'Name','Repetitions','Factor','TR','TE','TI','TD','ReadBW',...
                        'Nx','Ny','Nz','FOVx','FOVy','FOVz','Gmax','SlewRate'};

handles.Values={...
        {'','','',''};...
        {''};...
        {'','','B2E','','','',''};...
        {'','0',''};...
        {'','90','0','0.1',''};...
        {'','90','0','1','4','0.5','','','10','INTERLEAVED'};...
        {'','GX','','','','',''};...
        {'','GZ','','',''};...
        {'','GX','','','','',''};...
        {'','GY','1','','','','',''};...
        {'','GX','40','32','0.2','1'};...
        {'','0.1','1','2','0','180','','',''};...
        {'','0.1','1','50',''};...
        {'',''};...
        {'','','',''}...
    } ;
handles.Parameter={'TR','TE','TI','TD','ReadBW','Nx','Ny','Nz','FOVx','FOVy','FOVz','Gmax','SlewRate'};

for i=1:length(handles.Modules)
    eval(['handles.ModuleAtrributes.',handles.Modules{i},'=[];']) %filled by plotSeqTree button presses
end

set(handles.addADCs,'Visible','off');
for i=1:16
    eval(['set(handles.SOtag',num2str(i),',''Visible'',''off'');'])
    eval(['set(handles.SOEtag',num2str(i),',''Visible'',''off'');'])
end

global INSERT_MODULE_NUMBER
INSERT_MODULE_NUMBER=0;

%create the toolbar
handles.icons=load(fullfile(handles.JemrisPath,'ModuleIcons'));
handles.hpt=seqcad_uitoolbar(hObject,handles);

guidata(hObject, handles);

% --- Outputs from this function are returned to the command line.
function varargout = JEMRIS_seq3_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles;

% --- runs jemris on hte current seqence. This is *not* an object of the GUI!
function call_jemris(hObject,handles)

%cluster call
[dummy,SUBDIR]=fileparts(handles.CWD);
unixcommand=sprintf('ssh localhost "cd %s ; rm -f seqout.txt; %s/jemris %s > seqout.txt"',...
                    handles.CWD,handles.JemrisPath,handles.seqfile);                
[status,dump]=unix(unixcommand);
C={};
fid=fopen('seqout.txt');
while 1
    tline = fgetl(fid);
    if ~ischar(tline), break, end
    C{end+1}=tline;
end
fclose(fid);
set(handles.seq_dump,'String',C);
guidata(hObject, handles);

% --- Executes on button press in update.
function update_Callback(hObject, eventdata, handles)
writeXMLseq(handles.Seq,fullfile(handles.CWD,handles.seqfile));
set(handles.SeqNameTag,'String',['Sequence: ',handles.seqfile])
for i=1:length(handles.hpt);
    set(handles.hpt{i},'State','off'); 
end
call_jemris(hObject,handles);
if get(handles.kspace_flag,'Value')
    handles.Seq=plotseq(handles,1,1+get(handles.addADCs,'Value'));
else
    handles.Seq=plotseq(handles,handles.dm);
end
handles.SeqBackup=handles.Seq;
set(handles.zoomFlag,'Value',0);
guidata(hObject, handles);

% --- Executes on button press in draw_moments.
function draw_moments_Callback(hObject, eventdata, handles)
handles.dm=get(hObject,'Value');
handles.Seq=plotseq(handles,handles.dm);
set(handles.zoomFlag,'Value',0);
set(handles.kspace_flag,'Value',0);
set(handles.show_interval,'Visible','on')
set(handles.text8,'Visible','on')
set(handles.addADCs,'Visible','off')
guidata(hObject, handles);

% --- Executes on button press in undo.
function undo_Callback(hObject, eventdata, handles)
save jseq_handles handles %TMPTMP
handles.Seq=parseXMLseq(handles.seqfile);
guidata(hObject, handles);
update_Callback(hObject, eventdata, handles)
if 0
 [a,b,c]=fileparts(handles.seqfile);
 D=dir([b,'*_seq*.pdf']);
 pdfname=sprintf('%s_seq%03d',b,length(D)+1);
 set(gcf,'PaperPositionMode','auto','InvertHardcopy','off')
 print('-dpdf',pdfname)
end

% --- Executes on setting the interval in show_interval
function show_interval_Callback(hObject, eventdata, handles)
handles.ax=str2num(get(hObject,'String'));
if isempty(handles.ax),handles.ax=0;end
[handles.Seq,handles.ax]=plotseq(handles,handles.dm);
set(handles.show_interval,'String',num2str(handles.ax));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function show_interval_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on selection change in seq_dump.
function seq_dump_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function seq_dump_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in zoomFlag.
function zoomFlag_Callback(hObject, eventdata, handles)
if get(hObject,'Value')
    zoom(gcf,'on')
else
    if get(handles.kspace_flag,'Value')
        handles.Seq=plotseq(handles,1,1+get(handles.addADCs,'Value'));
    else
        handles.Seq=plotseq(handles,handles.dm);
    end
    zoom(gcf,'off')
end


% --- Executes on button press in kspace_flag.
function kspace_flag_Callback(hObject, eventdata, handles)
handles.dm=0;
if get(hObject,'Value')
    handles.Seq=plotseq(handles,1,1+get(handles.addADCs,'Value'));
    set(handles.draw_moments,'Value',0);
    set(handles.show_interval,'Visible','off')
    set(handles.addADCs,'Visible','on')
    set(handles.text8,'Visible','off')
    set(handles.zoomFlag,'Value',0);
else
    handles.Seq=plotseq(handles,handles.dm);
    set(handles.show_interval,'Visible','on')
    set(handles.addADCs,'Visible','off')
    set(handles.text8,'Visible','on')
    set(handles.zoomFlag,'Value',0);
end
guidata(hObject, handles);


% --- Executes on button press in addADCs.
function addADCs_Callback(hObject, eventdata, handles)
if get(hObject,'Value')
    handles.Seq=plotseq(handles,1,2);
    set(handles.zoomFlag,'Value',0);
else
    handles.Seq=plotseq(handles,1,1);
    set(handles.zoomFlag,'Value',0);
end
guidata(hObject, handles);


% --------------------------------------------------------------------
function FileTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function OpenSeqTag_Callback(hObject, eventdata, handles)
[FileName,PathName] = uigetfile('*.xml','Select the Sequence XML file');
if FileName==0,return;end
handles.seqfile=FileName;
handles.Seq=parseXMLseq(handles.seqfile);
handles.SeqBackup=handles.Seq;
handles.ax=0;
handles.dm=0;
set(handles.draw_moments,'Value',0);
call_jemris(hObject,handles);
[handles.Seq,handles.ax]=plotseq(handles,handles.dm);
set(handles.show_interval,'String',num2str(handles.ax));
set(handles.zoomFlag,'Value',0);
set(handles.kspace_flag,'Value',0);
set(handles.show_interval,'Visible','on')
set(handles.text8,'Visible','on')
set(handles.addADCs,'Visible','off')
set(handles.SeqNameTag,'String',['Sequence: ',FileName])
guidata(hObject, handles);

% --------------------------------------------------------------------
function SaveSeqTag_Callback(hObject, eventdata, handles)
[FileName,PathName] = uiputfile('*.xml');
if FileName==0,return;end
handles.seqfile=FileName;
guidata(hObject, handles);
update_Callback(hObject, eventdata, handles);


% --------------------------------------------------------------------
function NewSeqTag_Callback(hObject, eventdata, handles)
[FileName,PathName] = uiputfile('*.xml');
if FileName==0,return;end
S.Name='ConcatSequence'; S.Attributes.Name='Name';
S.Attributes.Value='New Sequence'; S.Data='';
P.Name='Parameter'; P.Attributes(1).Name='';
P.Attributes(1).Value=''; P.Data=[]; P.Children=[]; P.current=0;
S.Children(1)=P;  S.current=1;
handles.seqfile=FileName;
handles.Seq=S;
hax=handles.hax;for i=1:length(hax); cla(hax{i},'reset'); set(hax{i},'visible','off'); end
guidata(hObject, handles);
update_Callback(hObject, eventdata, handles);

% --------------------------------------------------------------------
function PlotTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function PlotGUITag_Callback(hObject, eventdata, handles)
 [FileName,PathName] = uiputfile('*.jpg');
 if FileName==0,return;end
 set(gcf,'PaperPositionMode','auto','InvertHardcopy','off')
 print('-djpeg90',FileName)

% --------------------------------------------------------------------
function PlotPDTag_Callback(hObject, eventdata, handles)
h=figure;
    p=[ 0.1300    0.7093    0.3347    0.2157;...
        0.5703    0.7093    0.3347    0.2157;...
        0.1300    0.4096    0.3347    0.2157;...
        0.5703    0.4096    0.3347    0.2157;...
        0.1300    0.1100    0.3347    0.2157;...
        0.5703    0.1100    0.3347    0.2157];
    I=[1 3 5 2 4 6];
    for j=1:6
         g(j)=copyobj(handles.hax{j},h);
         set(g(j),'units','normalized','position',p(I(j),:))
    end
    
%this function is performed for all text-edit callbacks on the module card
function common_SOEtags(hObject,handles)
S=get(hObject,'Tag');S(3)=[];
eval(['handles.Seq=changeSeqAttributes(get(handles.',S,...
      ',''String''),get(hObject,''String''),handles.Seq,handles,1);']);
%special case of external pulseshapes
if strcmp(eval(['get(handles.',S,',''String'')']),'FileName')
  s=sprintf('binary filename, or global variable\nfrom which the binary file is generated');
  set(hObject,'TooltipString',s)
else
  set(hObject,'TooltipString','')
end

axes(handles.hax{7}); cla(handles.hax{7},'reset');
handles.Seq=plotSeqTree(handles.Seq,handles);
guidata(hObject, handles);

function SOEtag1_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag1_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag2_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

function SOEtag2_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag3_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag3_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag4_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag4_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag5_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag5_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag6_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag6_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag7_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag7_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag8_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag8_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag9_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag9_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag10_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag10_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag11_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag11_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag12_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag12_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag13_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag13_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag14_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag14_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag15_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag15_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function SOEtag16_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag16_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


