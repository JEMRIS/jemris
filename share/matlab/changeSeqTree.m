function Seq=changeSeqTree(Seq,handles,root)
%
% changeSeqTree.m helper function of JEMRIS_seq.m
%

%
%  JEMRIS Copyright (C) 
%                        2006-2013  Tony Stoecker
%                        2007-2013  Kaveh Vahedipour
%                        2009-2013  Daniel Pflugfelder
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
 global INSERT_MODULE_NUMBER MODULE1 MODULE2 MODULE_TYPE_COUNTER
 switch INSERT_MODULE_NUMBER
    case -1 %delete a node
        if ~isempty(Seq.Children)
            if strcmp(upper(Seq.Name),'PARAMETERS')
                errordlg('Delete of Parameter node is not possible!');
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
            if ( length([findstr('ATOM',upper(MODULE1.Name)) findstr('ATOM',upper(MODULE2.Name))])==2 ) || ...% any 2 atoms can be swapped
               ( isempty([findstr('ATOM',upper(MODULE1.Name)) findstr('SEQUENCE',upper(MODULE1.Name))]) && ...% any 2 pulses can be swapped
                  isempty([findstr('ATOM',upper(MODULE2.Name)) findstr('SEQUENCE',upper(MODULE2.Name))]) )
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
            
            ispulse=~isempty(findstr('PULSE',upper(MODULE1.Name)));
            if  ( ispulse && ~strcmp('ATOMICSEQUENCE',upper(Seq.Name) ) ) || ...
                (~ispulse && ~strcmp('CONCATSEQUENCE',upper(Seq.Name) ) )
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
 static_atom = 0;
 if INSERT_MODULE_NUMBER>0 
      eval(['modname=''',handles.Modules{INSERT_MODULE_NUMBER},''';']);
      ispulse=strcmp(handles.ModType{INSERT_MODULE_NUMBER},'PULSE');
      isatom =strcmp(handles.ModType{INSERT_MODULE_NUMBER},'ATOM');
      if  ( isatom  && strcmp('PARAMETERS',upper(Seq.Name)) )
          static_atom = 1;
          if numel(Seq.Children)>1
            NewModule=[];
            warndlg('Static Atom module is already inserted in Parameter node!')
            return;          
          end

      elseif  ( ispulse && ~strcmp('ATOMICSEQUENCE',upper(Seq.Name)) ) || ...
              (~ispulse && ~strcmp('CONCATSEQUENCE',upper(Seq.Name)) )      
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
          if ( ~strcmp(V{i},'0') && ~strcmp(A{i},'SlewRate') && ~strcmp(A{i},'MaxAmpl') )
              eval(['NewModule.Attributes(i).Name=''',A{i},''';'])
              NewModule.Attributes(i).Value=V{i};
          end
      end

      j=0;
      switch upper(handles.ModType{INSERT_MODULE_NUMBER})
              case 'CONCAT'
                  j=1; sNAME='C';
              case 'ATOM'
                  j=2; sNAME='A';
                  if strcmp(upper(handles.Values{INSERT_MODULE_NUMBER}{1}(1:5)),'DELAY'),j=3;sNAME='D';end
              case 'PULSE'
                  j=4; sNAME='P';
              otherwise
                  disp(['unkown module: ',s])
      end
      MODULE_TYPE_COUNTER(j)=MODULE_TYPE_COUNTER(j)+1;
      sNAME=sprintf('%s%d',sNAME,MODULE_TYPE_COUNTER(j));
      if (static_atom), sNAME='SA';end
      NewModule.Attributes(1).Value=sNAME;
      NewModule.current=1;NewModule.hp=0;NewModule.hl=0;NewModule.hi=0;NewModule.ht=0;
 end
