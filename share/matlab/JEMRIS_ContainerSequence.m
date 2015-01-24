function varargout = JEMRIS_ContainerSequence(varargin)
%GUI for jemris ContainerSequence tree design

%
%  JEMRIS Copyright (C)
%                        2006-2014  Tony Stoecker
%                        2007-2014  Kaveh Vahedipour
%                        2009-2014  Daniel Pflugfelder
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

% Begin initialization code - DO NOT EDIT
gui_Singleton = 0;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @JEMRIS_ContainerSequence_OpeningFcn, ...
                   'gui_OutputFcn',  @JEMRIS_ContainerSequence_OutputFcn, ...
                   'gui_LayoutFcn',  [ ] , ...
                   'gui_Callback',   [ ]);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before JEMRIS_seq is made visible.
function JEMRIS_ContainerSequence_OpeningFcn(hObject, eventdata, handles, varargin)
colordef white

% Choose default command line output for JEMRIS_seq
handles.output = hObject;

%ContainerSeq xml file
global CONTAINERSEQFILE
if length(CONTAINERSEQFILE)<1
    warndlg('Specify filename of the ContainerSequence');
    close(hObject)
    return;
end
handles.seqfile = CONTAINERSEQFILE;
handles.seqdir  = pwd;
handles.Seq=[ ];       % the sequence structure
handles.SeqBackup=[ ]; % a backup

handles.SO=struct('Attribute',cell(10,1),'Value',cell(10,1)); %sequence object properties
hax{7}=handles.axes0; 
set(hax{7},'color',[1 1 1],'visible','off');
handles.hax=hax;
handles.dm=0;
handles.cd=0;
handles.ax=0;
handles.CWD=pwd;
handles.win = (strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64'));
if (handles.win)
    if (strcmp(computer,'PCWIN64'))
        handles.JemrisPath=['"',winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Wow6432Node\Research Centre Juelich\jemris', 'Path')];
    else
        handles.JemrisPath=['"',winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Research Centre Juelich\jemris', 'Path')];
    end
    handles.JemrisShare=[''];
    handles.JemrisCall=[fullfile(handles.JemrisPath,'jemris.exe"')];
else
    handles.JemrisPath='/Users/stoeckert/DZNE/workspace/jemris/src';
    handles.JemrisShare='/Users/stoeckert/Documents/MATLAB/jemris';
    [s,w]=system('setenv');
    if s==0 % a TCSH
        handles.JemrisCall=['setenv LD_LIBRARY_PATH ""; ',fullfile(handles.JemrisPath,'jemris')];
    else    % a BASH 
        handles.JemrisCall=['LD_LIBRARY_PATH=""; PATH=/usr/local/bin:$PATH;',fullfile(handles.JemrisPath,'jemris')];
    end
end

%get all modules + attributes which are currently supported by JEMRIS
[Modules,Params]=getAllModules(handles.JemrisCall,handles.CWD);
for i=1:length(Modules)
    handles.ModType{i}   = Modules(i).type; 
    handles.Modules{i}   = Modules(i).name; 
    handles.Attributes{i}= Modules(i).attr.name; 
    handles.Values{i}    = Modules(i).attr.val;
    handles.HiddenAttributes{i}= Modules(i).hidden_attr; 
end
handles.Parameter=Params.a.name;
handles.ParameterHidden=Params.ha;
%sort: put first all gradient pulses to the end
NM=length(handles.Modules);
I=strfind(handles.Modules,'GRAD');
J=[];K=[];for i=1:NM; if ~isempty(I{i}),J(end+1)=i;else;K(end+1)=i;end,end
I=[K J];
handles.Modules=handles.Modules(I);
handles.ModType=handles.ModType(I);
handles.HiddenAttributes=handles.HiddenAttributes(I);
handles.Attributes=handles.Attributes(I);
handles.Values=handles.Values(I);
%sort: then put all RF pulses to the end
NM=length(handles.Modules);
I=strfind(handles.Modules,'RF');
J=[];K=[];for i=1:NM; if ~isempty(I{i}),J(end+1)=i;else;K(end+1)=i;end,end
I=[K J];
handles.Modules=handles.Modules(I);
handles.ModType=handles.ModType(I);
handles.HiddenAttributes=handles.HiddenAttributes(I);
handles.Attributes=handles.Attributes(I);
handles.Values=handles.Values(I);

set(handles.HiddenAttr,'Visible','off');

for i=1:17
    eval(['set(handles.SOtag',num2str(i),',''Visible'',''off'');'])
    eval(['set(handles.SOEtag',num2str(i),',''Visible'',''off'');'])
end

global INSERT_MODULE_NUMBER MODULE_TYPE_COUNTER
INSERT_MODULE_NUMBER=0;
MODULE_TYPE_COUNTER=[0 0 0 0 0];

%create the toolbar
handles.icons=load(fullfile(handles.JemrisShare,'ModuleIcons'));
handles.hpt=seqcad_uitoolbar(hObject,handles);

%read and plot sequence tree
set(handles.SeqNameTag,'String',['ContainerSequence: ',handles.seqfile])
axes(hax{7}); cla(hax{7},'reset');
if exist(handles.seqfile,'file')==2
    handles.Seq=parseXMLseq(handles);
else
    S.Name='ContainerSequence'; S.Attributes.Name='Name';
    S.Attributes.Value='CS'; S.Data=''; S.Children=[];  S.current=1;
    handles.Seq=S;
end

handles.Seq=plotSeqTree(handles.Seq,handles);
guidata(hObject, handles);



% --- Outputs from this function are returned to the command line.
function varargout = JEMRIS_ContainerSequence_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles;


% --- Executes on button press in update.
function update_Callback(hObject, eventdata, handles)
if (handles.win)
	  writeXMLseq(handles,handles.Seq, handles.seqfile);
else
    writeXMLseq(handles,handles.Seq,fullfile(handles.seqdir,handles.seqfile));
end
set(handles.SeqNameTag,'String',['ContainerSequence: ',handles.seqfile])
handles.CWD=pwd;
for i=1:length(handles.hpt);
    set(handles.hpt{i},'State','off'); 
end

handles.SeqBackup=handles.Seq;
hax=handles.hax;axes(hax{7}); cla(hax{7},'reset');
handles.Seq=plotSeqTree(handles.Seq,handles);
guidata(hObject, handles);

% --- Executes on button press in undo.
function undo_Callback(hObject, eventdata, handles)
handles.Seq=parseXMLseq(handles);
guidata(hObject, handles);
update_Callback(hObject, eventdata, handles)



    
%this function is performed for all text-edit callbacks on the module card
function common_SOEtags(hObject,handles)
S=get(hObject,'Tag');S(3)=[ ];
set(hObject,'TooltipString','')
%special case of adding a vector
if strcmp(eval(['get(handles.',S,',''String'')']),'Vector')
  s=sprintf('any matlab expression which evaluates to a vector');
  set(hObject,'TooltipString',s)
  v = eval(['[',get(hObject,'String'),']']); v=(v(:))';
  eval(['handles.Seq=changeSeqAttributes(get(handles.',S,...
        ',''String''),num2str(v),handles.Seq,handles);']);
else
  eval(['handles.Seq=changeSeqAttributes(get(handles.',S,...
        ',''String''),get(hObject,''String''),handles.Seq,handles);']);
end
%special case of external pulseshapes
if strcmp(upper(eval(['get(handles.',S,',''String'')'])),'FILENAME')
  s=sprintf('binary filename, or global variable\nfrom which the binary file is generated');
  set(hObject,'TooltipString',s)
end
%redraw tree
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

function SOEtag17_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function SOEtag17_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on selection change in HiddenAttr.
function HiddenAttr_Callback(hObject, eventdata, handles)
set(handles.HiddenAttr,'Value',1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function HiddenAttr_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
