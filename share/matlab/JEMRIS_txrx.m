function varargout = JEMRIS_txrx(varargin)
%GUI for jemris coil array layout

%
%  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

% Last Modified by GUIDE v2.5 12-Feb-2010 13:38:02

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @JEMRIS_txrx_OpeningFcn, ...
                   'gui_OutputFcn',  @JEMRIS_txrx_OutputFcn, ...
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


% --- Executes just before JEMRIS_txrx is made visible.
function JEMRIS_txrx_OpeningFcn(hObject, eventdata, handles, varargin)

colordef white

hax{1}=handles.axes1; hax{2}=handles.axes2; 
for i=1:length(hax);set(hax{i},'color',[1 1 1],'visible','off');end
handles.hax=hax;

%how to call jemris
handles.CWD=pwd;
handles.JemrisPath='/usr/local/bin';
handles.JemrisShare='/home/homeGlobal/stoecker/tony_ld1/Workspace/JEMRIS/share/matlab';
[s,w]=system('setenv');
if s==0 % a TCSH
    handles.JemrisCall=['setenv LD_LIBRARY_PATH ""; ',fullfile(handles.JemrisPath,'jemris')];
else    % a BASH
     handles.JemrisCall=['LD_LIBRARY_PATH=""; ',fullfile(handles.JemrisPath,'jemris')];
end

%fixed cloil proerties; should be automatically read from XML, as for
%modules, some day.
handles.Coils={'BIOTSAVARTLOOP','ANALYTICCOIL','EXTERNALCOIL'};
all={'Name','XPos','YPos','ZPos','Azimuth','Polar','Scale','Phase','Dim',...
     'Extent','Points'};
BSL=all;BSL(end+1)={'Radius'}; AC=all;AC(end+1)={'Sensitivity'};EC=all;EC(end+1)={'Filename'};
handles.Attributes={BSL,AC,EC};

% Choose default command line output for JEMRIS_txrx
handles.output = hObject;

set(handles.uipanel1,'visible','off')

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes JEMRIS_txrx wait for user response (see UIRESUME)
% uiwait(handles.figure1);

%external call to jemris !This is not a GUI function!
function handles=call_jemris(hObject,handles) 
 disp('External jemris call. Calculating sensitivity maps ... please wait.')
 [status,dump]=system(sprintf('%s %s > .coil.out',handles.JemrisCall,fullfile(handles.seqdir,handles.seqfile)));
 if status
     C{1}='Error occured:';
     C{2}='';
     C{3}=dump;
 else
  C={};
  fid=fopen('.coil.out');
  while 1
    tline = fgetl(fid);
    if ~ischar(tline), break, end
    C{end+1}=tline;
  end
  fclose(fid);     
 end
 for i=1:length(handles.CoilArray.Children)
     handles.CoilArray.Children(i).HasMap=1;
 end 
 handles=plotsensitivity(handles);
 guidata(hObject, handles);
 set(handles.listbox2,'String',C);
 set(handles.listbox2,'FontName','monospaced');
 set(handles.listbox2,'FontSize',8)
 guidata(hObject, handles);

% --- Outputs from this function are returned to the command line.
function varargout = JEMRIS_txrx_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles.output;

% --- Executes on selection change in ViewMenu.
function ViewMenu_Callback(hObject, eventdata, handles)
 handles=plotsensitivity(handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function ViewMenu_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in HideSampleMenu.
function HideSampleMenu_Callback(hObject, eventdata, handles)
 handles=plotsensitivity(handles);
 guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function HideSampleMenu_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in HideCoilMenu.
function HideCoilMenu_Callback(hObject, eventdata, handles)
 handles=plotsensitivity(handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function HideCoilMenu_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in DeleteCoil.
function DeleteCoil_Callback(hObject, eventdata, handles)
 for i=1:length(handles.CoilArray.Children)
     if handles.CoilArray.Children(i).current
         handles.CoilArray.Children(i)=[];
         handles=plotsensitivity(handles);
         guidata(hObject, handles);
         return
     end
 end
         
% --- Executes on button press in CopyCoil.
function CopyCoil_Callback(hObject, eventdata, handles)
 N=length(handles.CoilArray.Children);
 for i=1:N
     if handles.CoilArray.Children(i).current
         C=handles.CoilArray.Children(i);
         handles.CoilArray.Children(i).current=0;
         C.HasMap=0; C.hp=0; C.hl=0; C.ht=0;
         handles.CoilArray.Children(end+1)=C;
         handles=plotsensitivity(handles);
         guidata(hObject, handles);
         return
     end
 end

% --- Executes on selection change in CoilMenu.
function CoilMenu_Callback(hObject, eventdata, handles)


% --- Executes on button press in AddCoil.
function AddCoil_Callback(hObject, eventdata, handles)
N=length(handles.CoilArray.Children);
for i=1:N
    handles.CoilArray.Children(i).current=0;
end
n=get(handles.CoilMenu,'Value');
NewCoil = struct('Name', cell(1), 'Attributes',cell(1),'Data',cell(1),    ...
                 'Children',cell(1),'current', cell(1),'HasMap', cell(1), ...
                 'hp',cell(1), 'hl',cell(1),'CoilName',cell(1), 'ht', cell(1));
NewCoil.Name=handles.Coils{n};
NewCoil.Attributes.Name='Name';
NewCoil.Attributes.Value=num2str(N+1);
NewCoil.CoilName=num2str(N+1);
NewCoil.current=1;
NewCoil.HasMap=0;
NewCoil.hp=0; NewCoil.hl=0; NewCoil.ht=0;
if length(handles.CoilArray.Children)
    handles.CoilArray.Children(end+1)=NewCoil;
else
    handles.CoilArray.Children=NewCoil;
end
handles=plotsensitivity(handles);
guidata(hObject, handles);



% --- Executes during object creation, after setting all properties.
function CoilMenu_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in ArrayMenu.
function ArrayMenu_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function ArrayMenu_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in ReadXml.
function ReadXml_Callback(hObject, eventdata, handles)
handles.CoilArray=parseXMLseq(handles);
handles=call_jemris(hObject, handles);
guidata(hObject, handles);

% --- Executes on button press in WriteXml.
function WriteXml_Callback(hObject, eventdata, handles)
writeXMLseq(handles,handles.CoilArray,fullfile(handles.seqdir,handles.seqfile));
handles=call_jemris(hObject, handles);
guidata(hObject, handles);

% --------------------------------------------------------------------
function PlotGui_Callback(hObject, eventdata, handles)
 [FileName,PathName] = uiputfile('*.jpg');
 if FileName==0,return;end
 set(gcf,'PaperPositionMode','auto','InvertHardcopy','off')
 print('-djpeg90',FileName)

% --------------------------------------------------------------------
function NewFigure_Callback(hObject, eventdata, handles)
 h=figure('name','Sensitivity Map','numbertitle','off');
 g=copyobj(handles.hax{1},h);
 set(g,'units','normalized','position',[.1 .1 .8 .8])
 grid on
 set(h,'color',[1 1 1],'paperunits','centimeter','paperposition',[0 0 12 8])
    
% --------------------------------------------------------------------
function OpenFile_Callback(hObject, eventdata, handles)
[FileName,PathName] = uigetfile('*.xml','Select the Coil Array XML file');
if FileName==0,return;end
handles.seqfile=FileName;
handles.seqdir=PathName;
handles.CoilArray=parseXMLseq(handles);
set(handles.XmlFile,'string',FileName);
handles=call_jemris(hObject, handles);
guidata(hObject, handles);

% --------------------------------------------------------------------
function SaveFile_Callback(hObject, eventdata, handles)
[FileName,PathName] = uiputfile('*.xml');
if FileName==0,return;end
handles.seqfile=FileName;
handles.seqdir=PathName;
guidata(hObject, handles);
set(handles.XmlFile,'string',FileName);
WriteXml_Callback(hObject, eventdata, handles);

% --------------------------------------------------------------------
function NewFile_Callback(hObject, eventdata, handles)
[FileName,PathName] = uiputfile('*.xml');
if FileName==0,return;end
handles.seqfile=FileName;
handles.seqdir=PathName;
P.Name='CoilArray'; P.Attributes=struct([]);
P.Data=[ ]; P.Children=struct([]); P.current=0;
handles.CoilArray = P;
set(handles.XmlFile,'string',FileName);
hax=handles.hax;for i=1:length(hax); cla(hax{i},'reset'); set(hax{i},'visible','off'); end
guidata(hObject, handles);
%update_Callback(hObject, eventdata, handles);


% --------------------------------------------------------------------
function FigureTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function FileTag_Callback(hObject, eventdata, handles)



% --------------------------------------------------------------------
function LayoutTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function CylinderTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function SphereTag_Callback(hObject, eventdata, handles)



% --- Executes on selection change in ComplexMenu.
function ComplexMenu_Callback(hObject, eventdata, handles)
 handles=plotsensitivity(handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function ComplexMenu_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function slicepos_Callback(hObject, eventdata, handles)
 plotsensitivity(handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function slicepos_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

% --- Executes on selection change in listbox2.
function listbox2_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function listbox2_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in ApplyToAll.
function ApplyToAll_Callback(hObject, eventdata, handles)
CA=handles.CoilArray;
for i=1:length(CA.Children), handles.CoilArray.Children(i).current = ~handles.CoilArray.Children(i).current; end
eval(handles.LastChange);
for i=1:length(CA.Children), handles.CoilArray.Children(i).current = ~handles.CoilArray.Children(i).current; end
plotCoils(handles);
guidata(hObject, handles);

%this function is performed for all text-edit callbacks on the coil card
function common_SOEtags(hObject,handles)
S=get(hObject,'Tag');%S(11:end)=[ ];
S=strrep(S,'Edit','Text');
handles.LastObject=hObject;
STR=['handles.CoilArray=ChangeAttrib(get(handles.',S,...
      ',''String''),get(handles.LastObject,''String''),handles);'];
handles.LastChange = STR;
eval(STR);
%eval(['get(handles.',S,',''String'')']),get(hObject,'String')
%redraw
if strcmpi(eval(['get(handles.',S,',''String'')']),'Scale') 
    plotsensitivity(handles);
else
    plotCoils(handles);
end    
guidata(hObject, handles);

function EditAttrib1_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib1_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib2_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib2_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib3_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib3_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib4_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib4_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib5_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib5_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib6_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib6_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib7_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib7_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib8_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib8_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function EditAttrib9_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib9_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib10_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib10_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib11_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib11_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib12_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib12_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function EditAttrib13_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib13_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib14_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib14_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib15_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib15_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function EditAttrib16_Callback(hObject, eventdata, handles)
common_SOEtags(hObject,handles)

% --- Executes during object creation, after setting all properties.
function EditAttrib16_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



%changing coil attributes (not GUI function)
function CA=ChangeAttrib(AttrName,AttrVal,handles)

CA=handles.CoilArray;
for i=1:length(CA.Children)
    if CA.Children(i).current
       pos=0;
       eval(['n=find(strcmp(''',upper(CA.Children(i).Name),''',handles.Coils));'])
       A=handles.Attributes{n};
       for j=1:length(A);
           if strcmp(AttrName,A{j}), pos=-j; end
       end
       %attributes which are already assigned
       for j=1:length(CA.Children(i).Attributes);
           if strcmp(AttrName,CA.Children(i).Attributes(j).Name), pos=j; end
       end
       if pos>0
           if isempty(AttrVal)
               CA.Children(i).Attributes(pos)=[];              %delete attribute
           else
               CA.Children(i).Attributes(pos).Value=AttrVal;   %overwrite old value
           end
       end
       if pos<0 && ~isempty(AttrVal) %add new name/value pair
           CA.Children(i).Attributes(end+1).Name=AttrName;
           CA.Children(i).Attributes(end).Value=AttrVal;
       end
       %special case of external sensitivity map
       if ~isempty(strfind(upper(CA.Children(i).Name),'EXTERNAL')) && strcmpi(AttrName,'filename')
            if exist(AttrVal,'file') == 2, return, end %file exists!
            try
              eval(['global ',AttrVal])
              eval(['val= ',AttrVal,';'])
              val(1);
            catch
                disp(['No such global variable: ',AttrVal])
                return
            end
            %assign matrix size
            Ns=size(val); dim=length(Ns);
            if ~isempty(find(diff(Ns)))
                disp('external sensitivites must be square or cube');
                return;
            end
            for j=1:length(CA.Children(i).Attributes)
              hasdim=0; haspoints=0;
               if strcmpi('dim',CA.Children(i).Attributes(j).Name)
                  CA.Children(i).Attributes(j).Value=num2str(dim); 
                  hasdim=1;
               end
               if strcmpi('points',CA.Children(i).Attributes(j).Name)
                  CA.Children(i).Attributes(j).Value=num2str(Ns(1)); 
                  haspoints=1;
               end
            end
            if ~hasdim
             CA.Children(i).Attributes(end+1).Name='Dim';
             CA.Children(i).Attributes(end).Value=num2str(dim);
            end
            if ~haspoints
             CA.Children(i).Attributes(end+1).Name='Points';
             CA.Children(i).Attributes(end).Value=num2str(Ns(1));
            end
                
            %write file
            FileName=[AttrVal,'.bin'];
            f=fopen(FileName,'w','l');
            fwrite(f,val(:),'double');
            fclose(f);
            for j=1:length(CA.Children(i).Attributes)
                if strcmpi('filename',CA.Children(i).Attributes(j).Name)
                    CA.Children(i).Attributes(j).Value=FileName;
                end
            end
       end
    end
end
        
