function [modules,params]=getAllModules(Jcall,Jpath)
%[modules,params]=getAllModules(Jcall,Jpath)
%helper function of the JEMRIS_seq GUI - called only once at GUI opening
% gets all modules with all attributes from jemris through system call [Jcall,'modlist']
% moves result, mod.xml, to the matlab path of jemris (Jpath)

%
%  JEMRIS Copyright (C) 
%                        2006-2015  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2015  Daniel Pflugfelder
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

if ~isempty(Jcall)
 [status,dump]=system(sprintf('%s modlist ; mv mod.xml %s',Jcall,Jpath));
end
 h.seqfile = 'mod.xml';
 h.seqdir  = Jpath;
 S=parseXMLseq(h);
 modules=struct([ ]);

 for i=1:length(S.Children)
    c=S.Children(i);
    
        
    if strcmp(upper(c.Name),'PARAMETERS')
        [params.a,params.ha]=get_attributes(c.Attributes);
        continue;
    end
        
    for j=1:length(c.Children)
        cc=c.Children(j);
        modules(end+1).name=cc.Name;
        [modules(end).attr,modules(end).hidden_attr]=get_attributes(cc.Attributes);
        modules(end).type=c.Name(1:end-1);
    end

 end

 return

%%%
function [a,ha]=get_attributes(A)

 N=length(A);
 j=1;
 k=0;
 a.name = cell(0,1);
 a.val  = cell(0,1);
 ha     = cell(0,1);
 for i=1:N
    if isempty(strfind(A(i).Name,'HIDDEN'))
      a.name{end+1} = A(i).Name;
      if strcmp('NAME',upper(a.name{end})), j=length(a.name); end
      if strcmp('OBSERVE',upper(a.name{end})), k=length(a.name); end
      a.val{end+1}  = A(i).Value;
      if strcmp(A(i).Name,'Axis')
        switch A(i).Value
            case '0'
                a.val{end}  = 'RF';
            case '1'
                a.val{end}  = 'GX';
            case '2'
                a.val{end}  = 'GY';
            case '3'
                a.val{end}  = 'GZ';
            otherwise
                a.val{end}  = 'NONE';
        end
      end
    else
        ha{end+1} = A(i).Name;
    end
 end
 N=length(a.name);
 %NAME is always the first attribute !
 I=[j [1:j-1] [j+1:N]];
 a.name = a.name(I);
 a.val  = a.val(I) ;

 %and OBSERVE is always the second !
 if k>1
  I=[1 k [2:k-1] [k+1:N]];
  a.name = a.name(I);
  a.val  = a.val(I) ;
 end
 
return
