function BrainSample=brainSample(handles)
%
% brainSample.m helper function of JEMRIS_sim.m
%

%
%  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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
dim = 3;
slices          = handles.sample.R;
BrainSample.RES = handles.sample.DxDy;
interp_factor   = 1./BrainSample.RES;
add_susceptiblity = handles.sample.Suscept;

load(fullfile(handles.JemrisShare,'MNIbrain.mat'))

if length(slices)==1;slices=[slices slices];end
if isempty(slices);slices=[1 size(BRAIN,3)];end

d=find([1 2 3]~=dim); %[d(2) d(1)] are the dimensions for plotting
BRAIN=permute(BRAIN,[d(1) d(2) dim]);
BRAIN=BRAIN(:,:,slices(1):slices(2));

if add_susceptiblity
  load(fullfile(handles.JemrisShare,'MNIdeltaB.mat'))
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
%        T1  T2 T2*[ms]  M0 CS[rad/sec]      Label
tissue=[2569 329  158   1.00   0         ;  % 1 = CSF
         833  83   69   0.86   0         ;  % 2 = GM
         500  70   61   0.77   0         ;  % 3 = WM
         350  70   58   1.00 220*2*pi    ;  % 4 = Fat (CS @ 1.5 Tesla)
         900  47   30   1.00   0         ;  % 5 = Muscle / Skin
        2569 329   58   1.00   0         ;  % 6 = Skin
           0   0    0   0.00   0         ;  % 7 = Skull
         833  83   69   0.86   0         ;  % 8 = Glial Matter
         500  70   61   0.77   0         ;];% 9 = Meat

%parameter maps
PARAMS={'M0','T1','T2','T2S','DB'};
fact=[handles.sample.M0 handles.sample.T1 handles.sample.T2 handles.sample.T2S handles.sample.CS];
INDEX =[4 1 2 3 5];
for i=1:9
 for j=1:5
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
