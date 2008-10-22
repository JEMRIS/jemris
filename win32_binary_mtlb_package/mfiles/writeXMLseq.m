function writeXMLseq(handles,S,dnode,docNode)
%
%writeXMLseq(handles,S,XML_filename)
%recursevely create DOM from sequence structure, then write XML file
%
global MODULE_TYPE_COUNTER;

%TS@IME 03/2007

% Create DOM root node
if nargin==3
 MODULE_TYPE_COUNTER=[0 0 0 0];
 [p,n,x]=fileparts(dnode);
 if isempty(p),p=pwd;end
 xmlFileName = [p,'/',n,'.xml'];
 docNode = com.mathworks.xml.XMLUtils.createDocument(S.Name);
 dnode = docNode.getDocumentElement;
 if strcmp('CONCATSEQUENCE',upper(S.Name)) % this is a sequence: first set parameters
    thisElement = docNode.createElement('Parameter');
    dnode.appendChild(thisElement);
    
 end
end


%
if isfield(handles,'Modules')
 s = upper(S.Name);
 sNAME ='';
 for i=1:length(handles.Modules)
      if strcmp(upper(handles.Modules{i}),s)
          switch upper(handles.ModType{i})
              case 'CONCAT'
                  j=1; sNAME = 'C';
              case 'ATOM'
                  j=2; sNAME = 'A';
                  if strcmp(upper(handles.Values{i}{1}(1:5)),'DELAY'),j=3;sNAME='D';end
              case 'PULSE'
                  j=4; sNAME = 'P';
              otherwise
                  disp(['unkown module: ',s])
           end
           MODULE_TYPE_COUNTER(j)=MODULE_TYPE_COUNTER(j)+1;
           sNAME = sprintf('%s%d',sNAME,MODULE_TYPE_COUNTER(j));
       end
  end
end


%set attributes for this node
has_name_tag = 0;
for i=1:length(S.Attributes)
    if strcmp(upper(S.Attributes(i).Name),'NAME'), has_name_tag=1; end
    if ~isempty(S.Attributes(i).Value)
        dnode.setAttribute(S.Attributes(i).Name,S.Attributes(i).Value);
    end
end

%set attribute name, if the node has'nt one
if ~has_name_tag && isfield(handles,'Modules')
    dnode.setAttribute('Name',sNAME);
    fprintf('%s %s %d\n',S.Name,sNAME,has_name_tag)
end

%recursive call to set children
for i=1:length(S.Children)
    thisElement = docNode.createElement(S.Children(i).Name);
    dnode.appendChild(thisElement);
    dChildren=dnode.getChildNodes;
    dChild=dChildren.item(i-1);
    writeXMLseq(handles,S.Children(i),dChild,docNode)
end

%save XML document
if nargin==3
 xmlwrite(xmlFileName,dnode);
end