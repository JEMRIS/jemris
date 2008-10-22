function BrainSample=brainSample(handles)

%TS@IME-FZJ, 09/2008

dim = 3;
slices          = handles.sample.R;
BrainSample.RES = handles.sample.DxDy;
interp_factor   = 1./BrainSample.RES;
add_susceptiblity = handles.sample.Suscept;

load(fullfile(handles.JemrisPath,'MNIbrain.mat'))

if length(slices)==1;slices=[slices slices];end
if isempty(slices);slices=[1 size(BRAIN,3)];end

d=find([1 2 3]~=dim); %[d(2) d(1)] are the dimensions for plotting
BRAIN=permute(BRAIN,[d(1) d(2) dim]);
BRAIN=BRAIN(:,:,slices(1):slices(2));

if add_susceptiblity
  load(fullfile(handles.JemrisPath,'MNIdeltaB.mat'))
  DB=DB(:,:,round([slices(1):slices(2)]/2));
  Nx=size(DB,1); x=([0:Nx-1]-Nx/2+0.5);
  Ny=size(DB,2); y=([0:Ny-1]-Ny/2+0.5);
  Nz=size(DB,3); z=([0:Nz-1]-Nz/2+0.5);
  X=[x(1):(x(Nx)-x(1))/(2*Nx-1):x(Nx)];
  Y=[y(1):(y(Ny)-y(1))/(2*Ny-1):y(Ny)];
  if Nz==1
    DB = interp2(y,x,DB,Y',X,'nearest');
  else
    Z=[z(1):(z(Nz)-z(1))/(2*Nz-1):z(Nz)];
    DB = interp3(y,x,z,DB,Y',X,Z,'nearest');
 end 
end

Nx=size(BRAIN,1); x=([0:Nx-1]-Nx/2+0.5);
Ny=size(BRAIN,2); y=([0:Ny-1]-Ny/2+0.5);
Nz=size(BRAIN,3); z=([0:Nz-1]-Nz/2+0.5);

%interpolation
if interp_factor~=1
 X=[x(1):(x(Nx)-x(1))/(round(interp_factor*Nx)-1):x(Nx)];
 Y=[y(1):(y(Ny)-y(1))/(round(interp_factor*Ny)-1):y(Ny)];
 if Nz==1
    BRAIN=interp2(y,x,BRAIN,Y',X,'nearest');
    if add_susceptiblity; DB = interp2(y,x,DB,Y',X,'nearest'); end
else
    Z=[z(1):(z(Nz)-z(1))/(round(interp_factor*Nz)-1):z(Nz)];
    BRAIN=interp3(y,x,z,BRAIN,Y',X,Z,'nearest');
    if add_susceptiblity; DB = interp3(y,x,z,DB,Y',X,Z,'nearest'); end
 end
end

%tissuse parameters
%        T1  T2 T2*[ms]  M0 CS[Hz]      Label
tissue=[2569 329   58   1.00   0    ;  % 1 = CSF
         833  83   69   0.86   0    ;  % 2 = GM
         500  70   61   0.77   0    ;  % 3 = WM
         350  70   58   1.00 220    ;  % 4 = Fat
         900  47   30   1.00   0    ;  % 5 = Muscle / Skin
        2569 329   58   1.00   0    ;  % 6 = Skin
           0   0    0   0.00   0    ;  % 7 = Skull
         833  83   69   0.86   0    ;  % 8 = Glial Matter
         500  70   61   0.77   0    ;];% 9 = Meat

%parameter maps
PARAMS={'M0','T1','T2','DB'};
fact=[handles.sample.M0 handles.sample.T1 handles.sample.T2 handles.sample.CS];
INDEX =[  4   1    2     5];
for i=1:9
 for j=1:4
  if i==1,eval(['BrainSample.',PARAMS{j},'=zeros(size(BRAIN));']);end
  I   = find(BRAIN==i);
  ind = INDEX(j);
  eval(['BrainSample.',PARAMS{j},'(I)=fact(j)*tissue(i,ind);']);
 end
end

%add suceptibility
if add_susceptiblity
  BrainSample.DB = BrainSample.DB + 1e6*DB*handles.sample.gamBo;
end

return;
