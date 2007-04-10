function Seq=changeSeqAttributes(AttrName,AttrVal,Seq,handles,root);

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
   if root && length(Seq.Children)>0
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
   return;
end

%recursion over tree
for i=1:length(Seq.Children)
    Seq.Children(i)=changeSeqAttributes(AttrName,AttrVal,Seq.Children(i),handles,0);    
end

%%% END of function changeSeqAttributes %%%

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
 
   