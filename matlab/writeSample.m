function maxM0 = writeSample(varargin)
%
% writeSample - writes a sample binary file for jemris
% helper function of JEMRIS_sim.m
%
% usage:
%
% 1. writeSample(Sample) where Sample is a structure with 
%    fields: M0,T1,T2,DB,NN,RES,OFFSET,FNAME}
%     - M0,T1,T2,DB,NN have to be data cubes of the same size
%     - DB is delta_B (off-resoance in Hz !), NN is not used so far by
%       jemris ; if not specified, zero(size(M0)) is used for both.
%     - RES and OFFSET are resolution and offsets in 3 dimension
%       ! if RES and OFFSET, ofs are scalar, the same value is used for
%         all directions
%       ! if not specified, the default is RES=1 and OFFSET=0 
%     - FNAME is a string for the binary filename; default 'sample.bin'
%
% 2. writeSample(Shape,dim,res,ofs,M0,T1,...)
%    - Shape is one of 'Sphere' , '2Spheres', (... to be extended)
%    - dim, res, ofs are 3-elem-vectors (dimensions, resolution,offset)
%      ! if dim is scalar, a 2D sample of size (dim x dim) is created
%      ! if res, ofs are scalar, the same value is used for all directions
%      ! if ofs is empty, it is automatically the center of the data-cube
%

%
%  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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
if (isstruct(varargin{1}))
    Sample=varargin{1};
else
    Sample=getShape(varargin);
end


%check structure
if ~isfield(Sample,'RES')   , Sample.RES=1   ;             end
if ~isfield(Sample,'OFFSET'), Sample.OFFSET=0;             end
if ~isfield(Sample,'FNAME') , Sample.FNAME = 'sample.bin'; end
if ~isfield(Sample,{'M0','T1','T2'})
    error('fields missing in input structure')
end
if ~isfield(Sample,'DB'), Sample.DB=zeros(size(Sample.M0)); end
if ~isfield(Sample,'NN'), Sample.NN=zeros(size(Sample.M0)); end


%write binary file
f=fopen(Sample.FNAME,'w','a');

[Nx,Ny,Nz]=size(Sample.M0);
N=[Nx Ny Nz];

for i=1:3; 
    fwrite(f,N(i),'double');
    fwrite(f,Sample.RES(min([i length(Sample.RES)])),'double');
    fwrite(f,Sample.OFFSET(min([i length(Sample.OFFSET)])),'double');
end

A(:,:,:,1)=Sample.M0;
I=find(Sample.T1); R1=zeros(size(Sample.T1)); R1(I)=1./Sample.T1(I);
I=find(Sample.T2); R2=zeros(size(Sample.T2)); R2(I)=1./Sample.T2(I);
A(:,:,:,2)=R1;
A(:,:,:,3)=R2;
A(:,:,:,4)=Sample.DB;
A(:,:,:,5)=Sample.NN;

%save 4D data array for JEMRIS in the order (type , X, Y, Z)
A=permute(A,[4 1 2 3]); 
fwrite(f,A,'double');

fclose(f);

maxM0 = max(Sample.M0(:));

return;

%%%
function Sample=getShape(VA)
%try
        if length(VA)==10, fname=VA{10}; end
        dim    = VA{2};
        A      = zeros(max(dim));
        [nx,ny,nz]=size(A);
        [I,J,K]=meshgrid(1:nx,1:ny,1:nz); 
        M0=1;T1=1000;T2=1000;DB=0;NN=0;
        S={'M0','T1','T2','DB','NN'};
        switch(lower(VA{1}))
            case '2d sphere'
                R=dim(1)/2;
                if nz>1
                    L=find((I-R).^2+(J-R).^2+(K-R).^2 < R^2);
                else
                    L=find((I-R).^2+(J-R).^2 < R^2);
                end
                A(L)=1;
                for i=1:5
                 if length(VA)>3+i
                 eval([S{i},'=VA{4+i}(1);']);
                 end
                 eval([S{i},'=',S{i},'*A;']);
                end
               if length(VA)<10, fname=sprintf('sphere_%d.bin',length(L)); end
            case '2d 2-spheres'
                %outer sphere
                R=max(dim)/2;
                if nz>1
                    L=find((I-R).^2+(J-R).^2+(K-R).^2 < R^2);
                else
                    L=find((I-R).^2+(J-R).^2 < R^2);
                end
                A(L)=1;
                for i=1:5
                 if length(VA)>3+i
                 eval([S{i},'=VA{4+i}(1);']);
                 end
                 eval([S{i},'=',S{i},'*A;']);
                end
                %inner sphere
                Ri=min(dim)/2;
                if nz>1
                    L=find((I-R).^2+(J-R).^2+(K-R).^2 < Ri^2);
                else
                    L=find((I-R).^2+(J-R).^2 < Ri^2);
                end
                for i=1:5
                 if length(VA)>3+i
                  s=VA{4+i}(min([2 length(VA{4+i})]));
                  eval([S{i},'(L)=s;']);
                 end
                end
                if length(VA)<10, fname=sprintf('sphere_%d.bin',length(L)); end
            otherwise
                error('unkown pre-defined sample string')
        end

        res    = VA{3};
        offset = VA{4};
        
    %catch
    %    error('input is neither a sample-structure, nor a pre-defined sample')
    %end
 
    Sample=struct('M0',M0,'T1',T1,'T2',T2,'DB',DB,'NN',NN,'RES',res,'OFFSET',offset,'FNAME',fname);

return;
