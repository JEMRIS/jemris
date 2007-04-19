function Seq=changeSeqTree(Seq,handles,root)
global  MODULE1 MODULE2 

%add child to root, if root is current
if root && Seq.current
 NewChild=ChangeMe(Seq,handles);
 if isstruct(NewChild)
     Seq.current=0;
     Seq.Children(end+1)=NewChild;
     reset_togglebuttons(handles);
 end
 return
end
            

%recursion to find and change node(s) 
for i=1:length(Seq.Children)
    Seq.Children(i)=changeSeqTree(Seq.Children(i),handles,0);    
    if Seq.Children(i).current
        NewChild=ChangeMe(Seq.Children(i),handles);
        % different possibilities of changing
        if isstruct(NewChild) % 1. insert module
            Seq.Children(i).current=0;
            if isempty(Seq.Children(i).Children)
                Seq.Children(i).Children=NewChild;
            else
                Seq.Children(i).Children(end+1)=NewChild;
            end
            reset_togglebuttons(handles);
            break
        elseif NewChild==-1  % 2. deletes module
            Seq.Children(i)=[];
            reset_togglebuttons(handles);
            break
        elseif NewChild==-2  % 3. swaps modules
            S=swap_modules(MODULE1,MODULE2,handles.Seq);
            reset_togglebuttons(handles);
            MODULE1=S; MODULE2='swapped';
            break
        end
    end
end

if root && strcmp('swapped',MODULE2)
    Seq=MODULE1;
    MODULE1=0;
    MODULE2=0;
end

%%%%
function reset_togglebuttons(handles)
 for i=1:length(handles.hpt);
    set(handles.hpt{i},'State','off'); 
 end

%%%%
function S=swap_modules(M1,M2,S)
 for i=1:length(S.Children)
     if M1.hp == S.Children(i).hp
         S.Children(i)=M2;
     elseif M2.hp == S.Children(i).hp
         S.Children(i)=M1;
     else
         S.Children(i)=swap_modules(M1,M2,S.Children(i));
     end
 end

%%% 
function P=get_parent(M,S,root)
  global PARENT
  if root; PARENT=[]; end
  for i=1:length(S.Children)
    if M.hp == S.Children(i).hp
        PARENT=S;
    end
    get_parent(M,S.Children(i),0);
  end
  if root; P=PARENT; end

%%%%
function NewModule=ChangeMe(Seq,handles)
 global INSERT_MODULE_NUMBER MODULE1 MODULE2
 switch INSERT_MODULE_NUMBER
    case -1 %delete a node
        if ~isempty(Seq.Children)
            if strcmp(Seq.Children(1).Name,'Parameter')
                errordlg('Delete of root node is not possible!');
                NewModule=[];
                return;
            end
            button = questdlg('Delete this node with all its children?');
            if ~strcmp(button,'Yes'),NewModule=0;return,end
        end
        NewModule=-1;
        return
    case -2 %swap modules
        if isstruct(MODULE1) && ~isstruct(MODULE2),MODULE2=Seq; end %select the second module
        if ~isstruct(MODULE1),MODULE1=Seq; MODULE2=0; end           %select the first module
        if isstruct(MODULE1) && isstruct(MODULE2)                   %perform swap            
            if ( length([findstr('Atom',MODULE1.Name) findstr('Atom',MODULE2.Name)])==2 ) || ...% any 2 atoms can be swapped
               ( isempty([findstr('Atom',MODULE1.Name) findstr('Sequence',MODULE1.Name)]) && ...% any 2 pulses can be swapped
                  isempty([findstr('Atom',MODULE2.Name) findstr('Sequence',MODULE2.Name)]) )
                NewModule = -2;
                return
            else      %swap of sequence modules with the same parent is possible as well !
              P1=get_parent(MODULE1,handles.Seq,1);
              P2=get_parent(MODULE2,handles.Seq,1);
              if isstruct(P1) && isstruct(P2)
                  if P1.hp == P2.hp
                      NewModule = -2;
                      return;
                  end
              end
            end
            errordlg(sprintf(['Can not swap modules of different type!\n',...
                              'Both need to be either Atoms or Pulses\n',...
                              'or children of the same parent.']))
            MODULE1=0; MODULE2=0;
        end
        NewModule=[];
        return
     case -3 %copy module
        if isstruct(MODULE1)
            ispulse=isempty([findstr('Atom',MODULE1.Name) findstr('Sequence',MODULE1.Name)]);
            if  ( ispulse && ~strcmp('AtomicSequence',Seq.Name) ) || ...
                (~ispulse && ~strcmp('ConcatSequence',Seq.Name) )
                NewModule=[];
                warndlg(sprintf(['Copy of module %s into module \n',...
                                 'of type %s is not possible!'],modname,Seq.Name))
                return;
            end
            if ~isempty(MODULE1.Children)
                if strcmp(MODULE1.Children(1).Name,'Parameter')
                    MODULE1.Children(1)=[];
                end
            end
            NewModule=MODULE1;
            INSERT_MODULE_NUMBER=1000;
            return;
        end
        if ~isstruct(MODULE1),MODULE1=Seq; end           %select the module to copy
        NewModule=[];
        return
end
 
 %insert a module
 if INSERT_MODULE_NUMBER>0 
      eval(['modname=''',handles.Modules{INSERT_MODULE_NUMBER},''';']);
      ispulse=isempty([findstr('Atom',modname) findstr('Sequence',modname)]);
      if  ( ispulse && ~strcmp('AtomicSequence',Seq.Name) ) || ...
          (~ispulse && ~strcmp('ConcatSequence',Seq.Name) )
          NewModule=[];
          warndlg(sprintf(['Insert of module %s into module \n',...
                           'of type %s is not possible!'],modname,Seq.Name))
          return;
      end
      NewModule = struct('Name', cell(1), 'Attributes',cell(1),    ...
      'Data',cell(1), 'Children',cell(1), 'current', cell(1), ...
      'hp',cell(1), 'hl',cell(1), 'hi', cell(1));
      NewModule.Name=modname;
      A=handles.Attributes{INSERT_MODULE_NUMBER};
      V=handles.Values{INSERT_MODULE_NUMBER};
      for i=1:length(A)
          eval(['NewModule.Attributes(i).Name=''',A{i},''';'])
          NewModule.Attributes(i).Value=V{i};
      end
      NewModule.Attributes(1).Value=modname;
      NewModule.current=1;NewModule.hp=0;NewModule.hl=0;NewModule.hi=0;
 end