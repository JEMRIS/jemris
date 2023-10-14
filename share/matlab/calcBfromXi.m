function B=calcBfromXi(S,Bo);
% B=calcBfromXi(S,Bo);
% calcualte magnetic field inhomogeneities for a given susceptibility distribution
% Input
%	S  : 3D data cube of suceptibility-values
%       Bo : strength main field (unit Tesla), default 1.5
%
% Output
%	B  : Field inhomogeneities
%
% Method is taken from
%	Marques, J.P. and Bowtell, R.
%	"Application of a fourier-based method for rapid calculation of field inhomogeneity
%	 due to spatial variation of magnetic susceptibility",
%	Concepts in Magnetic Resonance Part B - MR Engineering,2005, 25B (1),65-78
%


%
%  JEMRIS Copyright (C) 
%                        2006-2023  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2019  Daniel Pflugfelder
%                                  
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

if nargin<2,Bo=1.5;end %Tesla

if nargin==0
 % homogenous sphere (radius N/4 , suscept.=1e-4)  
 N=128; %cube size is N*N*N
 S=zeros(N,N,N);
 [x,y,z]=meshgrid(-N/2+1:N/2,-N/2+1:N/2,-N/2+1:N/2);
 I=find(x.^2+y.^2+z.^2 < N^2/16);
 S(I)=1e-4;
else
 for i=1:3, N(i)=size(S,i); M(i)=2^nextpow2(N(i)); end
 s=S; S=zeros(M); S(1:N(1),1:N(2),1:N(3))=s;
end

% differential operator in frequency domain 
[Nx,Ny,Nz]=size(S);
fx=[0:1/Nx:.5]; fx=[fx -fliplr(fx(2:end-1))];   %freq. axis suitable for FFT
fy=[0:1/Ny:.5]; fy=[fy -fliplr(fy(2:end-1))];   %freq. axis suitable for FFT
fz=[0:1/Nz:.5]; fz=[fz -fliplr(fz(2:end-1))];   %freq. axis suitable for FFT
[kx,ky,kz]=ndgrid(fx,fy,fz);
warning off
Dk=(kz.^2)./(kx.^2+ky.^2+kz.^2);    %oops, devide by zero
warning on
Dk(1,1,1)=0;                        %we don't care and set the NaN to zero

%calculate field
 B=Bo*real(ifftn((1/3-Dk).*fftn(S)));

if nargin~=0, B=B(1:N(1),1:N(2),1:N(3)); end

