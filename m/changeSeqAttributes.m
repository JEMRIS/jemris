function Seq=changeSeqAttributes(AttrName,AttrVal,Seq,handles,root)

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
 
   %special case of the root node's parameter tag
   if root && ~isempty(Seq.Children)
      if strcmp('Parameter',Seq.Children(1).Name)
           pos=getPosition(AttrName,Seq.Children(1),handles,1);
        if pos>0
            if isempty(AttrVal)
               Seq.Children(1).Attributes(pos)=[];              %delete attribute
            else
               Seq.Children(1).Attributes(pos).Value=AttrVal;   %overwrite old value
            end
        end
        if pos<0 && ~isempty(AttrVal) %add new name/value pair
            Seq.Children(1).Attributes(end+1).Name=AttrName;
            Seq.Children(1).Attributes(end).Value=AttrVal;
        end
      end
   end

   %special case of external pulse shapes
   if strcmp(Seq.Name,'ExternalPulseShape') && strcmp(AttrName,'FileName')
       if exist(AttrVal,'file') == 2, return, end %file exists!
       eval(['global ',AttrVal])
       eval(['val= ',AttrVal,';'])
       [N,M]=size(val);
       AX='';
       for i=1:length(Seq.Attributes)
           if strcmp(Seq.Attributes(i).Name,'Axis')
               AX=Seq.Attributes(i).Value;
           end
           if strcmp(Seq.Attributes(i).Name,'FileName')
               iFN=i;
           end
       end
       
       if strcmp(AX,'GX') || strcmp(AX,'GY') || strcmp(AX,'GZ')
           if M~=2,
               uiwait(warndlg('Gradient pulses must be Nx2 vectors ([time,Amplitude]','warning','modal'))
               return
           end
       elseif strcmp(AX,'RF')
           if M~=3,
               uiwait(warndlg('RF pulses must be Nx3 vectors ([time,Magnitude,Phase]','warning','modal'))
               return
           end
       else
           uiwait(warndlg('specify axis first!','warning','modal'))
           return
       end
       
       writeExtPulseBinFile(AttrVal,val)
       Seq.Attributes(iFN).Value=[AttrVal,'.bin'];
   end
   
   return;
end

%recursion over tree
for i=1:length(Seq.Children)
    Seq.Children(i)=changeSeqAttributes(AttrName,AttrVal,Seq.Children(i),handles,0);    
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
function pos=getPosition(AttrName,Seq,handles,parametertag)
 pos=0;
 
 %all attributes
 if nargin<4
    eval(['n=find(strcmp(''',Seq.Name,''',handles.Modules));'])
    A=handles.Attributes{n};
 else
     A=handles.Parameter;
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
 
   