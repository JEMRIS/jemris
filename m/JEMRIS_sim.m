function varargout = JEMRIS_sim(varargin)
%GUI for jemris simulation visualisation

%TS@IME-FZJ 03/2007

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @JEMRIS_sim_OpeningFcn, ...
                   'gui_OutputFcn',  @JEMRIS_sim_OutputFcn, ...
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


% --- Executes just before JEMRIS_sim is made visible.
function JEMRIS_sim_OpeningFcn(hObject, eventdata, handles, varargin)

colordef white

% Choose default command line output for JEMRIS_sim
handles.output = hObject;

handles.seqfile = '';
hax{1}=handles.axes1; hax{2}=handles.axes2;
hax{3}=handles.axes3; hax{4}=handles.axes4;
hax{5}=handles.axes5; hax{6}=handles.axes6;
for i=1:6; set(hax{i},'color',[1 1 1],'visible','off'); end
handles.hax=hax;
handles.epil=0;
handles.epir=0;
handles.CWD=pwd;
handles.JEMRISPATH=fileparts(which('JEMRIS_sim'));
sample.type='Sphere';
sample.T1=1000;sample.T2=100;sample.M0=1;sample.CS=0;
sample.R=50;sample.DxDy=1;
sim.DF=0; sim.CSF=0; sim.CF=0; sim.RN=0; sim.INC=0;
handles.sample=sample;
handles.UserSample=[];
handles.sim=sim;
handles.img_num=1;
%set(gcf,'color',[.88 .88 .88])

C={'Sample','Signal','k-Space','Image'};
set(handles.showLeft,'String',C);
C={'Signal','k-Space','Image','Evolution'};
set(handles.showRight,'String',C);
C={'Sphere','2Spheres','brain1','brain2','user defined'};
set(handles.Sample,'String',C);
set(handles.EPI_L,'Visible','off');
set(handles.EPI_R,'Visible','off');
set(handles.ImageL,'Visible','off');
set(handles.ImageR,'Visible','off');
set(handles.LocCompTag,'Checked','off');
set(handles.ParCompTag,'Checked','on');
write_simu_xml(handles,1);
guidata(hObject, handles);


% UIWAIT makes JEMRIS_sim wait for user response (see UIRESUME)
% uiwait(handles.figure1);

% --- writes the simulation xml file. This is *not* an object of the GUI!
function handles=write_simu_xml(handles,redraw)

RESOURCES=handles.JEMRISPATH;
if nargin<2 && strcmp(get(handles.ParCompTag,'Checked'),'on')
    RESOURCES='/apps/prod/misc/share/jemris';
end

sample=handles.sample;sim=handles.sim;
SIMU.Name='JMRI-SIM'; SIMU.Attributes=''; SIMU.Data='';
SAMPLE.Name='Sample'; SAMPLE.Attributes=''; SAMPLE.Data=''; SAMPLE.Children=[]; 
 %sample item
 switch sample.type
     case 'Sphere'
         NAMES = {'Shape','Radius','Delta','M0','T1','T2','CS'};
         VALUES= {'Sphere2D',num2str(sample.R),num2str(sample.DxDy),num2str(sample.M0),num2str(sample.T1),...
                  num2str(sample.T2),num2str(sample.CS)};
     case '2Spheres'
         NAMES = {'Shape','Delta','Radius_1','M0_1','T1_1','T2_1','CS_1','Radius_2','M0_2','T1_2','T2_2','CS_2'};
         VALUES= {'2Spheres2D',num2str(sample.DxDy),num2str(sample.R(1)),num2str(sample.M0(1)),...
                  num2str(sample.T1(1)),num2str(sample.T2(1)),num2str(sample.CS(1)),num2str(sample.R(2)),...
                  num2str(sample.M0(2)),num2str(sample.T1(2)),num2str(sample.T2(2)),num2str(sample.CS(2))};
    case 'brain1'
          NAMES = {'InFile'}; VALUES= {[RESOURCES,'/tra0mm_mr_Susc_CS.bin']};
    case 'brain2'
          NAMES = {'InFile'}; VALUES= {[RESOURCES,'/tra32mm_mr_Susc_CS.bin']};
    case 'user defined'
          if isempty(handles.UserSample)
           [FileName,PathName] = uigetfile('*.mat','Select the Sample Mat file');
           if FileName==0,return;end
           S=load(FileName);
           b=isfield(S,{'fname','M0','T1','T2','DB','dx','dy','dz'});
           if ~isempty(find(b==0, 1))
             error('Mat file has not the required fields (fname,M0,T1,T2,DB,dx,dy,dz)')
           end
           writeSample(S);
           handles.UserSample=S;
          end
          NAMES = {'InFile'}; VALUES= {handles.UserSample.fname};
 end
 for i=1:length(NAMES); SAMPLE.Attributes(i).Name=NAMES{i}; SAMPLE.Attributes(i).Value=VALUES{i}; end
 SIMU.Children(1)=SAMPLE;
 %model item
 MODEL.Name='Model'; MODEL.Attributes=''; MODEL.Data=''; MODEL.Children=[]; 
 if (sim.CF==0),CF=-1;else CF=1/sim.CF;end
 NAMES = {'FieldFluctuations','ChemicalShiftFactor','ConcomitantFields'};
 VALUES= {num2str(sim.DF),num2str(sim.CSF),num2str(CF)};
 for i=1:length(NAMES); MODEL.Attributes(i).Name=NAMES{i}; MODEL.Attributes(i).Value=VALUES{i}; end
 if (sim.INC>0),MODEL.Attributes(end+1).Name='SaveEvolution';MODEL.Attributes(end).Value=num2str(sim.INC);end
 SIMU.Children(2)=MODEL;
 %write simu xml file 
 writeXMLseq(SIMU,'simu.xml');

%redraw sample
if nargin==2
 unixcommand=sprintf('ssh localhost "cd %s; %s/jemris simu.xml"',handles.CWD,handles.JEMRISPATH);
 [status,dump]=unix(unixcommand);
 for i=[1 3 4 5 6]
    cla(handles.hax{i},'reset');
    set(handles.hax{i},'color',[1 1 1],'visible','off');
 end
 plotall(handles.hax,1,0,0,0);
 set(handles.showLeft,'Value',1);
end

% --- Outputs from this function are returned to the command line.
function varargout = JEMRIS_sim_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles;

% --- Executes on selection change in Sample.
function Sample_Callback(hObject, eventdata, handles)
 C=get(hObject,'String');
 Nsample=get(hObject,'Value');
 handles.sample.type=C{Nsample};
 switch Nsample
    case 1
        handles.sample.T1=1000;handles.sample.T2=100;handles.sample.M0=1;handles.sample.CS=0;
        handles.sample.R=50;handles.sample.DxDy=1;
        handles.sim.DF=0;handles.sim.CSF=1;
    case 2
        handles.sample.T1=[100 50];handles.sample.T2=[100 50];handles.sample.M0=[1 1];handles.sample.CS=[0 0];
        handles.sample.R=[50 25];handles.sample.DxDy=1;
        handles.sim.DF=0;handles.sim.CSF=1;
 end
 if Nsample>2;bvis='off';handles.sample.CS=handles.sample.CS(1);else bvis='on';end
 if Nsample>2;CSstr='CS fact';else CSstr='CS [kHz]';end
 set(handles.text17,'String',CSstr); 
 set(handles.setT1,'String',num2str(handles.sample.T1),'Visible',bvis);
 set(handles.setT2,'String',num2str(handles.sample.T2),'Visible',bvis);
 set(handles.setM0,'String',num2str(handles.sample.M0),'Visible',bvis);
 set(handles.setChemShift,'String',num2str(handles.sample.CS));
 set(handles.setRadius,'String',num2str(handles.sample.R),'Visible',bvis);
 set(handles.setGrid,'String',num2str(handles.sample.DxDy),'Visible',bvis);
 set(handles.setDeltaF,'String',num2str(handles.sim.DF));
 set(handles.text10,'Visible',bvis);
 set(handles.text11,'Visible',bvis);
 set(handles.text12,'Visible',bvis);
 set(handles.text13,'Visible',bvis);
 set(handles.text16,'Visible',bvis);
 if Nsample>4; bvis='off'; else,bvis='on';handles.UserSample=[]; end
 set(handles.text17,'Visible',bvis);
 set(handles.setChemShift,'Visible',bvis);
 %redraw sample
 handles=write_simu_xml(handles,1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function Sample_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in start_simu.
function start_simu_Callback(hObject, eventdata, handles)
if (exist(handles.seqfile) ~= 2); errordlg('select sequence first!'); return; end
write_simu_xml(handles);
if strcmp(get(handles.ParCompTag,'Checked'),'on')
 [dummy,SUBDIR]=fileparts(handles.CWD);
 %set up a PBS script on the cluster
 S_PBS=sprintf('sed -e ''s/XML_SEQ/%s/'' -e ''s/XML_SIM/simu.xml/'' /apps/prod/misc/share/jemris/pbs.script > mypbs',handles.seqfile);
 %command to launch command into queue on cluster 
 unixcommand=sprintf('ssh mrcluster "cd %s; rm -f simu.done out.txt; %s; qsub mypbs"',SUBDIR,S_PBS);
 C={'now executing',unixcommand,'','... wait for results'};
 set(handles.sim_dump,'String',C);
 guidata(hObject, handles);
 [status,dump]=unix(unixcommand);
 pause(1);
 %block matlab until result appears (?? is there a better way ??)
 while exist('simu.done')~=2,end
 unix(['ssh mrcluster "cd ',SUBDIR,'; rm -f mypbs simu.done *.ime462.tmp"']);
else
 unixcommand=sprintf('ssh localhost "cd %s; %s/jemris %s simu.xml > out.txt"',...
                     handles.CWD,handles.JEMRISPATH,handles.seqfile);
 C={'now executing',unixcommand,'','... wait for results'};
 set(handles.sim_dump,'String',C);
 guidata(hObject, handles);
 pause(1);
 [status,dump]=unix(unixcommand);
end

C={};
fid=fopen('out.txt');
while 1
    tline = fgetl(fid);
    if ~ischar(tline), break, end
    C{end+1}=tline;
end
fclose(fid);
set(handles.sim_dump,'String',C);
set(handles.showLeft,'Value',1);
set(handles.showRight,'Value',1);
guidata(hObject, handles);
showLeft_Callback(hObject, eventdata, handles);
showRight_Callback(hObject, eventdata, handles);

% --------------------------------------------------------------------
function FileTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function loadSeqTag_Callback(hObject, eventdata, handles)
[FileName,PathName] = uigetfile('*.xml','Select the Sequence XML file');
if FileName==0,return;end
handles.seqfile=FileName;
set(handles.SeqNameTag,'String',['Sequence: ',FileName]);
guidata(hObject, handles);

% --------------------------------------------------------------------
function loadSampleTag_Callback(hObject, eventdata, handles)
[FileName,PathName] = uigetfile('*.mat;*.bin','Select sample from mat file, or binary file');
if FileName==0,return;end
handles.samplefile=FileName;
guidata(hObject, handles);

% --- Executes on selection change in sim_dump.
function sim_dump_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function sim_dump_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setT1_Callback(hObject, eventdata, handles)
handles.sample.T1=str2num(get(hObject,'String'));
write_simu_xml(handles,1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setT1_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setT2_Callback(hObject, eventdata, handles)
handles.sample.T2=str2num(get(hObject,'String'));
write_simu_xml(handles,1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setT2_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setM0_Callback(hObject, eventdata, handles)
handles.sample.M0=str2num(get(hObject,'String'));
write_simu_xml(handles,1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setM0_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setChemShift_Callback(hObject, eventdata, handles)
handles.sample.CS=str2num(get(hObject,'String'));
if strcmp('on',get(handles.setM0,'visible'))
 write_simu_xml(handles,1);
end
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setChemShift_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setRadius_Callback(hObject, eventdata, handles)
handles.sample.R=str2num(get(hObject,'String'));
write_simu_xml(handles,1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setRadius_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setGrid_Callback(hObject, eventdata, handles)
handles.sample.DxDy=str2num(get(hObject,'String'));
write_simu_xml(handles,1);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setGrid_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setIncTag_Callback(hObject, eventdata, handles)
handles.sim.INC=str2num(get(hObject,'String'));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setIncTag_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setDeltaF_Callback(hObject, eventdata, handles)
handles.sim.DF=1e-3*str2num(get(hObject,'String'));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setDeltaF_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setConcField_Callback(hObject, eventdata, handles)
handles.sim.CF=str2num(get(hObject,'String'));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setConcField_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setNoise_Callback(hObject, eventdata, handles)
handles.sim.RN=str2num(get(hObject,'String'));
guidata(hObject, handles);
set(hObject,'Value',get(handles.showRight,'Value'));
showRight_Callback(hObject, eventdata, handles);
set(hObject,'Value',get(handles.showLeft,'Value'));
showLeft_Callback(hObject, eventdata, handles);
set(hObject,'Value',0);
set(handles.zoomflag,'Value',0);
 zoom(gcf,'off')
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function setNoise_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in showLeft.
function showLeft_Callback(hObject, eventdata, handles)
axes(handles.hax{1});
for i=[1 3 4 5 6]
    cla(handles.hax{i},'reset');
    set(handles.hax{i},'color',[1 1 1],'visible','off');
end
Nimg=plotall(handles.hax,get(hObject,'Value'),handles.epil,handles.sim.RN,handles.img_num);
if get(hObject,'Value')<3,bvis='off';else;bvis='on';end
set(handles.EPI_L,'Visible',bvis);
if (Nimg==1),bvis='off';
else;bvis='on';for i=1:Nimg;C{i}=['# ',num2str(i)];end;set(handles.ImageL,'String',C);end
set(handles.ImageL,'Visible',bvis);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function showLeft_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in showRight.
function showRight_Callback(hObject, eventdata, handles)
axes(handles.hax{2});
cla(handles.hax{2},'reset');
set(handles.hax{2},'color',[1 1 1],'visible','off');
Nimg=plotall(handles.hax,1+get(hObject,'Value'),handles.epir,handles.sim.RN,handles.img_num);
if get(hObject,'Value')==1,bvis='off';else;bvis='on';end
set(handles.EPI_R,'Visible',bvis);
if (Nimg==1),bvis='off';handles.img_num=1;
else;bvis='on';for i=1:Nimg;C{i}=['# ',num2str(i)];end;set(handles.ImageR,'Value',1),set(handles.ImageR,'String',C);end
set(handles.ImageR,'Visible',bvis);
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function showRight_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in EPI_R.
function EPI_R_Callback(hObject, eventdata, handles)
handles.epir=get(hObject,'Value');
set(hObject,'Value',get(handles.showRight,'Value'));
showRight_Callback(hObject, eventdata, handles);
set(hObject,'Value',handles.epir);
guidata(hObject, handles);

% --- Executes on button press in EPI_L.
function EPI_L_Callback(hObject, eventdata, handles)
handles.epil=get(hObject,'Value');
set(hObject,'Value',get(handles.showLeft,'Value'));
showLeft_Callback(hObject, eventdata, handles);
set(hObject,'Value',handles.epil);
guidata(hObject, handles);


% --- Executes on button press in zoomflag.
function zoomflag_Callback(hObject, eventdata, handles)
if get(hObject,'Value')
    zoom(gcf,'on')
else
 set(hObject,'Value',get(handles.showRight,'Value'));
 showRight_Callback(hObject, eventdata, handles);
 set(hObject,'Value',get(handles.showLeft,'Value'));
 showLeft_Callback(hObject, eventdata, handles);
 set(hObject,'Value',0);
 guidata(hObject, handles);
 zoom(gcf,'off')
end


% --- Executes on selection change in ImageL.
function ImageL_Callback(hObject, eventdata, handles)
handles.img_num=get(hObject,'Value');
set(hObject,'Value',get(handles.showLeft,'Value'));
showLeft_Callback(hObject, eventdata, handles);
set(hObject,'Value',handles.img_num);
guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function ImageL_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in ImageR.
function ImageR_Callback(hObject, eventdata, handles)
handles.img_num=get(hObject,'Value');
set(hObject,'Value',get(handles.showRight,'Value'));
showRight_Callback(hObject, eventdata, handles);
set(hObject,'Value',handles.img_num);
guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function ImageR_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function setROI_Callback(hObject, eventdata, handles)
r=str2num(get(hObject,'String'));
[x,y]=ginput(1);
try
 c=get(gca,'Children'); A=get(c(end),'Cdata');
 [X,Y]=size(A);[X,Y]=meshgrid(1:X,1:Y);
 [I,J]=find( (X-x).^2+(Y-y).^2 <= r^2 );
 if ~isempty(I); A=A(I,J); else A=A(round(x),round(y));end
 set(handles.MROI,'String',['M=',num2str(mean(A(:)),3)]);
 set(handles.SROI,'String',['S=',num2str(std(A(:)),3)]);
 hold on
  if r>0
   plot(x+r*cos(0:.01:2*pi),y+r*sin(0:.01:2*pi),'r','linewidth',2)
  else
   plot(x,y,'xr')
  end
 hold off
catch
 set(handles.MROI,'String','');
 set(handles.SROI,'String','');
end
guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function setROI_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --------------------------------------------------------------------
function plotTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function plotGUITag_Callback(hObject, eventdata, handles)
 [FileName,PathName] = uiputfile('*.jpg');
 if FileName==0,return;end
 set(gcf,'PaperPositionMode','auto','InvertHardcopy','off')
 print('-djpeg90',FileName)


% --------------------------------------------------------------------
function plotLeftTag_Callback(hObject, eventdata, handles)
h=figure;
colormap(gray);
if get(handles.showLeft,'Value')>1
 h=copyobj(handles.hax{1},h);
 set(h,'units','normalized','position',[.1 .1 .8 .8])
else
    p=[ 0.1300    0.5838    0.3347    0.3412; ...
        0.5703    0.5838    0.3347    0.3412; ...
        0.1300    0.1100    0.3347    0.3412; ...
        0.5703    0.1100    0.3347    0.3412];
    for j=1:4
         g(j)=copyobj(handles.hax{2+j},h);
         set(g(j),'units','normalized','position',p(j,:))
         colorbar('peer',g(j));
    end
end

% --------------------------------------------------------------------
function plotRightTag_Callback(hObject, eventdata, handles)
h=figure;
colormap(gray);
h=copyobj(handles.hax{2},h);
set(h,'units','normalized','position',[.1 .1 .8 .8])
if get(handles.showRight,'Value')==4;colorbar('peer',h,'southoutside');end

% --------------------------------------------------------------------
function SettingsTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function ParCompTag_Callback(hObject, eventdata, handles)
 set(hObject,'Checked','on')
 set(handles.LocCompTag,'Checked','off');
 guidata(hObject, handles);

% --------------------------------------------------------------------
function LocCompTag_Callback(hObject, eventdata, handles)
 set(hObject,'Checked','on')
 set(handles.ParCompTag,'Checked','off');
 guidata(hObject, handles);

% --------------------------------------------------------------------
function ComputationTag_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function loadSigTag_Callback(hObject, eventdata, handles)
 [FileName,PathName] = uigetfile('*.mat','Select a Signal mat-file');
 if FileName==0,return;end
 SIGNAL=load(FileName);
 f=fopen('signal.bin','wb'); fwrite(f,SIGNAL.All,'double','l'); fclose(f);
 cla(handles.hax{2},'reset');
 set(handles.hax{2},'color',[1 1 1],'visible','off');
 set(handles.EPI_R,'Visible','off');
 set(handles.ImageR,'Visible','off');
 plotall(handles.hax,2,handles.epir,handles.sim.RN,handles.img_num);
 set(handles.showRight,'Value',1);
 guidata(hObject, handles);
% --------------------------------------------------------------------
function saveSigTag_Callback(hObject, eventdata, handles)
 [FileName,PathName] = uiputfile('*.mat');
 if FileName==0,return;end
 f=fopen('signal.bin'); All=fread(f,Inf,'double','l');fclose(f);
 n=size(All,1)/4; A =reshape(All,4,n)';t=A(:,1);[t,I]=sort(t);M=A(I,2:4);
 d=diff(diff(t));d(d<1e-5)=0;I=[0;find(d)+1;length(t)];
 save(FileName,'t','M','I','All');
 


% --------------------------------------------------------------------
function saveMagEvolTag_Callback(hObject, eventdata, handles)
 [FileName,PathName] = uiputfile('*.mat');
 if FileName==0,return;end
 [M,x,y,t]=read_selex_evol(pwd);
 save(FileName,'t','M','x','y');

% hObject    handle to saveMagEvolTag (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


