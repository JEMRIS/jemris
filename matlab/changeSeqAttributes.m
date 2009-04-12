function Seq=changeSeqAttributes(AttrName,AttrVal,Seq,handles)
%
% changeSeqAttributes.m helper function of JEMRIS_seq.m
%

%
%  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

if Seq.current
    
   %general case
   pos=getPosition(AttrName,Seq,handles);
   %[pos length(Seq.Attributes)]
   if pos>0
       if isempty(AttrVal)
           Seq.Attributes(pos)=[];              %delete attribute
       else
           Seq.Attributes(pos).Value=AttrVal;   %overwrite old value
       end  
   end
   if pos<0 && ~isempty(AttrVal) %add new name/value pair
       Seq.Attributes(end+1).Name=AttrName;
       Seq.Attributes(end).Value=AttrVal;
   end
 
  
   %special case of external pulse shapes
   if ~isempty(strfind(upper(Seq.Name),'EXTERNAL')) && strcmp(upper(AttrName),'FILENAME')
       if exist(AttrVal,'file') == 2, return, end %file exists!
       eval(['global ',AttrVal])
       eval(['val= ',AttrVal,';'])
       [N,M]=size(val);
       AX='';
       IS_GRAD=0;
       for i=1:length(Seq.Attributes)
           if strcmp(upper(Seq.Attributes(i).Name),'AXIS')
               AX=Seq.Attributes(i).Value;
               IS_GRAD=1;
           end
           if strcmp(upper(Seq.Attributes(i).Name),'FILENAME')
               iFN=i;
           end
       end
       
       if IS_GRAD
           if M~=2,
               uiwait(warndlg('Gradient pulses must be Nx2 vectors ([time,Amplitude]','warning','modal'))
               return
           end
       else
           if M~=3,
               uiwait(warndlg('RF pulses must be Nx3 vectors ([time,Magnitude,Phase]','warning','modal'))
               return
           end
       end
       
       writeExtPulseBinFile(AttrVal,val)
       Seq.Attributes(iFN).Value=[AttrVal,'.bin'];
   end
   
   return;
end

%recursion over tree
for i=1:length(Seq.Children)
    Seq.Children(i)=changeSeqAttributes(AttrName,AttrVal,Seq.Children(i),handles);    
end

%%% END of function changeSeqAttributes %%%

%%%%%%%
function writeExtPulseBinFile(FileName,values)
 FileName=[FileName,'.bin'];
 [N,M]=size(values);
 values=real(values)';
 f=fopen(FileName,'w','l');
 fwrite(f,N,'double');
 fwrite(f,values(:),'double');
 fclose(f);

%%%%%%%
function pos=getPosition(AttrName,Seq,handles)
 pos=0;
 
 %all attributes
 eval(['n=find(strcmp(''',upper(Seq.Name),''',handles.Modules));'])
 if ~isempty(n);
    A=handles.Attributes{n};
 elseif strcmp(upper(Seq.Name),'PARAMETERS')
     A=handles.Parameter;
 else
     error(['unkown module: ',Seq.Name])
 end
 
 for i=1:length(A);
    if strcmp(AttrName,A{i})
        pos=-i;
    end
 end

 %attributes which are already assigned
 for i=1:length(Seq.Attributes);
        if strcmp(AttrName,Seq.Attributes(i).Name)
            pos=i;
        end
 end
 
   
