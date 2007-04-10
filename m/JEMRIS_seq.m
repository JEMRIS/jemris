function varargout = JEMRIS_seq(varargin)
%GUI for jemris sequence visualisation

%TS@IME-FZJ 03/2007

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @JEMRIS_seq_OpeningFcn, ...
                   'gui_OutputFcn',  @JEMRIS_seq_OutputFcn, ...
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


% --- Executes just before JEMRIS_seq is made visible.
function JEMRIS_seq_OpeningFcn(hObject, eventdata, handles, varargin)
colordef white
% Choose default command line output for JEMRIS_seq
handles.output = hObject;
handles.seqfile = 'FID.xml';
hax{1}=handles.axes1; hax{2}=handles.axes2; hax{3}=handles.axes3; hax{4}=handles.axes4; hax{5}=handles.axes5;
for i=1:5;set(hax{i},'color',[1 1 1],'visible','off');end
handles.hax=hax;
handles.dm=0;
handles.ax=0;
handles.CWD=pwd;

set(handles.addADCs,'Visible','off');
set(handles.file_name,'String',handles.seqfile);
guidata(hObject, handles);
%set(gcf,'color',[.88 .88 .88])

% --- Outputs from this function are returned to the command line.
function varargout = JEMRIS_seq_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles.output;

% --- runs jemris on hte current seqence. This is *not* an object of the GUI!
function call_jemris(hObject,handles)
[dummy,SUBDIR]=fileparts(handles.CWD);
unixcommand=sprintf('ssh tstoecker@mrcluster "cd %s ; rm -f out.txt; ./jemris %s > out.txt"',SUBDIR,handles.seqfile);
[status,dump]=unix(unixcommand);
C={};
fid=fopen('out.txt');
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
call_jemris(hObject,handles);
if get(handles.kspace_flag,'Value')
    plotseq(handles.seqfile,handles.hax,handles.ax,1,1+get(handles.addADCs,'Value'));
else
    plotseq(handles.seqfile,handles.hax,handles.ax,handles.dm);
end
set(handles.zoomFlag,'Value',0);
guidata(hObject, handles);

% --- Executes after string setting in file_name
function file_name_Callback(hObject, eventdata, handles)
handles.ax=0;
handles.dm=0;
set(handles.draw_moments,'Value',0);
handles.seqfile = get(hObject,'String');
call_jemris(hObject,handles);
handles.ax=plotseq(handles.seqfile,handles.hax,handles.ax,handles.dm);
set(handles.show_interval,'String',num2str(handles.ax));
set(handles.zoomFlag,'Value',0);
set(handles.kspace_flag,'Value',0);
set(handles.show_interval,'Visible','on')
set(handles.text8,'Visible','on')
set(handles.addADCs,'Visible','off')
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function file_name_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in draw_moments.
function draw_moments_Callback(hObject, eventdata, handles)
handles.dm=get(hObject,'Value');
plotseq(handles.seqfile,handles.hax,handles.ax,handles.dm);
set(handles.zoomFlag,'Value',0);
set(handles.kspace_flag,'Value',0);
set(handles.show_interval,'Visible','on')
set(handles.text8,'Visible','on')
set(handles.addADCs,'Visible','off')
guidata(hObject, handles);

% --- Executes on button press in save_plot.
function save_plot_Callback(hObject, eventdata, handles)
[a,b,c]=fileparts(handles.seqfile);
D=dir([b,'*_seq*.pdf']);
pdfname=sprintf('%s_seq%03d',b,length(D)+1);
set(gcf,'PaperPositionMode','auto','InvertHardcopy','off')
print('-dpdf',pdfname)

% --- Executes on setting the interval in show_interval
function show_interval_Callback(hObject, eventdata, handles)
handles.ax=str2num(get(hObject,'String'));
if isempty(handles.ax),handles.ax=0;end
handles.ax=plotseq(handles.seqfile,handles.hax,handles.ax,handles.dm);
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
        plotseq(handles.seqfile,handles.hax,handles.ax,1,1+get(handles.addADCs,'Value'));
    else
        plotseq(handles.seqfile,handles.hax,handles.ax,handles.dm);
    end
    zoom(gcf,'off')
end


% --- Executes on button press in kspace_flag.
function kspace_flag_Callback(hObject, eventdata, handles)
handles.dm=0;
if get(hObject,'Value')
    plotseq(handles.seqfile,handles.hax,handles.ax,1,1+get(handles.addADCs,'Value'));
    set(handles.draw_moments,'Value',0);
    set(handles.show_interval,'Visible','off')
    set(handles.addADCs,'Visible','on')
    set(handles.text8,'Visible','off')
    set(handles.zoomFlag,'Value',0);
else
    plotseq(handles.seqfile,handles.hax,handles.ax,handles.dm);
    set(handles.show_interval,'Visible','on')
    set(handles.addADCs,'Visible','off')
    set(handles.text8,'Visible','on')
    set(handles.zoomFlag,'Value',0);
end
guidata(hObject, handles);


% --- Executes on button press in addADCs.
function addADCs_Callback(hObject, eventdata, handles)
if get(hObject,'Value')
    plotseq(handles.seqfile,handles.hax,handles.ax,1,2);
    set(handles.zoomFlag,'Value',0);
else
    plotseq(handles.seqfile,handles.hax,handles.ax,1,1);
    set(handles.zoomFlag,'Value',0);
end
guidata(hObject, handles);


