%Set number of spins for each category
Nx_blood=100;
Nx_tissue1=0;
Nx_tissue2=0;


Nx0=Nx_tissue1;
Nx1=Nx_tissue1+Nx_tissue2;
Nx2=Nx_tissue1+Nx_tissue2+Nx_blood;


DB=zeros(Nx1,1,1);
M0=zeros(Nx1,1,1);
T1=zeros(Nx1,1,1);
T2=zeros(Nx1,1,1);
T2s=zeros(Nx1,1,1);


%Tissue 1
M0(1:Nx0,:,:)=1;
T1(1:Nx0,:,:)=144; %T1 water+glycerol = 1154 ms, T1 water+Glycerol+Gadolinium = 144 ms  (3 Tesla)
T2(1:Nx0,:,:)=10;   %T2 water+glycerol = 192 ms, T1 water+Glycerol+Gadolinium = 86 ms  (3 Tesla)
T2s(1:Nx0,:,:)=10;
DB(1:Nx0,:,:)=0;
OFFSET=[0 0 0];
RES=1;

%Tissue 2
M0(Nx0+1:Nx1,:,:)=1;
T1(Nx0+1:Nx1,:,:)=2885; %T1 water  2885 ms  (3 Tesla)
T2(Nx0+1:Nx1,:,:)=10;
T2s(Nx0+1:Nx1,:,:)=10;
DB(Nx0+1:Nx1,:,:)=0;
OFFSET=[0 0 0];
RES=1;

%Blood
M0(Nx1+1:Nx2,:,:)=1;
T1(Nx1+1:Nx2,:,:)=1584; %T1 Venous 1584 ms  Arterial 1664 ms  (3 Tesla)
T2(Nx1+1:Nx2,:,:)=10;  %T2 275 ms  (3 Tesla)
T2s(Nx1+1:Nx2,:,:)=10;
DB(Nx1+1:Nx2,:,:)=0;
OFFSET=[0 0 0];
RES=1;


save('MySample.mat','M0','T1','T2','T2s','OFFSET','RES','DB');
sample=load('MySample.mat');
writeSample(sample);
