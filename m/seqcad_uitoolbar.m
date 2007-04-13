function hpt=seqcad_uitoolbar(hObject,handles)

%TS@IME-FZJ 03/2007
ht = uitoolbar(hObject);
ic=handles.icons;
Modules=handles.Modules;
ModuleToolTip={'sequence container (ConcatSequence)','pulse container (AtomicSequence)','delay (DelayAtom)',...
               'empty pulse','hard RF pulse','sinc RF pulse','simple gradient pulse',...
               'slice select gradient','readout gradient','phase encode gradient table',...
               'RF phase cycling','RF spoiling'};
%draw buttons
NM=length(Modules);
for i=1:NM
    eval(['B=ic.',Modules{i},';']);       %the icon
    if i==4,bsep='on';else;bsep='off';end %separator for SEQ|PULSES
    if (i>3); B(find(isnan(B)))=1; end    %make background white for pulse icons
    handles.hpt{i} = uitoggletool(ht,'CData',B,'TooltipString',['Insert ',ModuleToolTip{i}],'Separator',bsep);
end
handles.hpt{NM+1} = uitoggletool(ht,'CData',ic.EraseModule,'TooltipString','erase a module','Separator','on');
CopyModule=permute(ic.SwapModules,[2 1 3]);
handles.hpt{NM+2} = uitoggletool(ht,'CData',CopyModule,'TooltipString','copy a module','Separator','on');
handles.hpt{NM+3} = uitoggletool(ht,'CData',ic.SwapModules,'TooltipString','swap two modules','Separator','on');

%define callbacks
for i=1:length(handles.hpt)
   if i<11
    eval(['set(handles.hpt{i},''OnCallback'',{@tbbdf_',Modules{i},',Modules{i},handles});']); 
   end
   if i==NM+1; set(handles.hpt{i},'OnCallback',{@tbbdf_Erase,handles}); end
   if i==NM+2; set(handles.hpt{i},'OnCallback',{@tbbdf_Copy,handles}); end
   if i==NM+3; set(handles.hpt{i},'OnCallback',{@tbbdf_Swap,handles}); end
   set(handles.hpt{i},'OffCallback',@tbbdf_Off);
end
hpt =handles.hpt;

%%%%%%%%%%%%%%%%%%%%%&%%%%%%%%%%%
%%%%%%  callback functions %%%%%%
%%%%%%%%%%%%%%%%%%%%%%&%%%%%%%%%%
function tbbdf_Erase(src,eventdata,handles)
tbbdf_common([],handles)
global INSERT_MODULE_NUMBER
INSERT_MODULE_NUMBER=-1;
function tbbdf_Swap(src,eventdata,handles)
tbbdf_common([],handles)
global INSERT_MODULE_NUMBER MODULE1 MODULE2
INSERT_MODULE_NUMBER=-2; MODULE1=0; MODULE2=0;
function tbbdf_Copy(src,eventdata,handles)
tbbdf_common([],handles)
global INSERT_MODULE_NUMBER MODULE1 MODULE2
INSERT_MODULE_NUMBER=-3; MODULE1=0; MODULE2=0;
function tbbdf_Off(src,eventdata)
global INSERT_MODULE_NUMBER MODULE1 MODULE2
INSERT_MODULE_NUMBER=0; MODULE1=0; MODULE2=0;

%all callbacks call this routine
function tbbdf_common(seq,handles)
global INSERT_MODULE_NUMBER
for i=1:length(handles.hpt)
    if (gcbo~=handles.hpt{i})
        set(handles.hpt{i},'State','off') %switch off all other modules
    else
        imn=i;
    end
end
INSERT_MODULE_NUMBER=imn;           %set current module active for insertion
guidata(handles.output, handles);

function tbbdf_AtomicSequence(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_ConcatSequence(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_DelayAtom(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_EmptyPulse(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_HardRfPulseShape(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_SincRfPulseShape(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_SS_TGPS(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_PE_TGPS(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_RfPhaseCycling(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_RfSpoiling(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_RO_TGPS(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

function tbbdf_TGPS(src,eventdata,seq,handles)
tbbdf_common(seq,handles)

