function writeXMLseq(S,dnode,docNode)
%
%writeXMLseq(S,XML_filename)
%recursevely create DOM from sequence structure, then write XML file
%

%TS@IME 03/2007

% Create DOM root node
if nargin==2
 [p,n,x]=fileparts(dnode);
 if isempty(p),p=pwd;end
 xmlFileName = [p,'/',n,'.xml'];
 docNode = com.mathworks.xml.XMLUtils.createDocument(S.Name);
 dnode = docNode.getDocumentElement;
end

%set attributes for this node
for i=1:length(S.Attributes)
    if ~isempty(S.Attributes(i).Value)
        dnode.setAttribute(S.Attributes(i).Name,S.Attributes(i).Value);
    end
end

%recursive call to set children
for i=1:length(S.Children)
    thisElement = docNode.createElement(S.Children(i).Name);
    dnode.appendChild(thisElement);
    dChildren=dnode.getChildNodes;
    dChild=dChildren.item(i-1);
    writeXMLseq(S.Children(i),dChild,docNode)
end

%save XML document
if nargin==2
 xmlwrite(xmlFileName,dnode);
end