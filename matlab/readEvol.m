function [M,t]=readEvol(fsample,fevols,do_plot)
%
% redEvol.m helper function of JEMRIS_sim.m
%

%
%  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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
if nargin==1, fevols='evol'; end
if nargin<3 , do_plot=1    ; end

%get dimensions
f=fopen(fsample); A=fread(f,Inf,'double'); fclose(f);
N=zeros(1,3); res=N; offset=N;
for i=1:3
    N(i)=A(1+(i-1)*3);
    res(i)=A(2+(i-1)*3);
    offset(i)=A(1+(i-1)*3);
end

A=A(10:end); A=reshape(A,[5 N]); A=permute(A,[2 3 4 1]); %permutes to (X,Y,Z,type)
I=find(A(:,:,:,1));
Np=length(I);

D=dir([fevols,'*.bin']);
Nt=length(D);
t=zeros(Nt,1);
M=zeros(N(1),N(2),N(3),3,Nt);

for i=1:length(D)
    f=fopen(D(i).name); B=fread(f,Inf,'double'); fclose(f);
    if (B(1) ~= Np), error('evol-sample-size does not match sample-size'),end
    t(i)=B(2);
    B=B(3:end); B=reshape(B,7,Np)';
    [dummy,J]=sort(B(:,1)); B=B(J,2:end);
    m=zeros(N(1),N(2),N(3));
    for j=1:3; m(I)=B(:,j+3); M(:,:,:,j,i)=m; end
end

if do_plot
T={'Mx','My','Mz'};
for i=1:Nt; 
    subplot(2,2,1)
    imagesc(A(:,:,:,1)),colorbar
    title('Sample M_{0}')
    for j=1:3; 
        subplot(2,2,j+1)
        imagesc(M(:,:,1,j,i)),colorbar
        title(sprintf('%s at t=%5.2f',T{j},t(i)))
    end
    if i<Nt
     disp(' - paused - press any key for next time step')
     pause
    end
 end
end

return;

