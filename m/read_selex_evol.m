function [M,x,y,t]=read_selex_evol(P)

if nargin<1,P=pwd;end

if ( ( exist('sim_data.bin','file')~=2 && exist('sim_data01.bin','file')~=2 ) || exist('sample.bin','file') ~=2)
    error('binary files do not exist')
end

%get sample dimensions
f=fopen([P,'/sample.bin']); A=fread(f,Inf,'double');fclose(f);
n=size(A,1)/8; A =reshape(A,8,n)';
d=unique(sort(A(:,6)));d=diff(d(1:2));
XYZ=round(A(:,6:8)/d);
X=XYZ(:,1);amx=abs(min(X));X=X+amx+1;
Y=XYZ(:,2);amy=abs(min(Y));Y=Y+amy+1;
Z=XYZ(:,3);amz=abs(min(Z));Z=Z+amz+1;
Nx=max(X);Ny=max(Y);Nz=max(Z);

%read timepoints of every node
SD=dir([P,'/sim_data*.bin']);SD=char(SD.name);
for node=1:size(SD,1)
	S=sprintf('%s/%s',P,SD(node,:));
 	%fprintf('reading file %s \n',S)
	f=fopen(S);
 	A=fread(f,Inf,'double'); fclose(f);
    N=A(end); A(end)=[];
	if node==1 %find number of timepoints and allocate output matrix
        Nt=size(A,1)/7/N; Is=[1:Nt];
		Is=[1:Nt];
		M=zeros(Nx*Ny,length(Is),3);
    end
    A=reshape(A,7,Nt,N);
	I=sub2ind([Nx Ny],X,Y);
 	M(I,:,1)=squeeze(A(5,Is,:))';
	M(I,:,2)=squeeze(A(6,Is,:))';
	M(I,:,3)=squeeze(A(7,Is,:))';
end
%spatial axes and time axis
M=reshape(M,Nx,Ny,length(Is),3);
t=A(1,Is,1);
x=([1:Nx]-Nx/2)*d;
y=([1:Nx]-Nx/2)*d;


