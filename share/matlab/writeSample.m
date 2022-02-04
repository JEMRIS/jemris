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
%     - DB is delta_B (off-resoance in rad/sec !), NN is not used so far by
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
%  JEMRIS Copyright (C)
%                        2006-2022  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2019  Daniel Pflugfelder
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
if ~isfield(Sample,'FNAME') , Sample.FNAME = 'sample.h5'; end
if ~isfield(Sample,{'M0','T1','T2'})
    error('fields missing in input structure')
end
if ~isfield(Sample,'T2S'),Sample.T2S=Sample.T2; end
if ~isfield(Sample,'DB'), Sample.DB =zeros(size(Sample.M0)); end
if ~isfield(Sample,'NN'), Sample.NN =zeros(size(Sample.M0)); end

if numel(Sample.RES)==1 Sample.RES=[1 1 1]*Sample.RES; end;
if numel(Sample.OFFSET)==1 Sample.OFFSET=[1 1 1]*Sample.OFFSET; end;

if numel(Sample.RES)==2 Sample.RES=[Sample.RES 1]; end;
if numel(Sample.OFFSET)==2 Sample.OFFSET=[Sample.OFFSET 0]; end;

%write binary file
A(:,:,:,1)=Sample.M0;
I=find(Sample.T1); R1 =zeros(size(Sample.T1));  R1(I) =1./Sample.T1(I);
I=find(Sample.T2); R2 =zeros(size(Sample.T2));  R2(I) =1./Sample.T2(I);
I=find(Sample.T2S);R2S=zeros(size(Sample.T2S)); R2S(I)=1./Sample.T2S(I);
A(:,:,:,2)=R1;
A(:,:,:,3)=R2;
A(:,:,:,4)=R2S;
A(:,:,:,5)=Sample.DB;

%save 4D data array for JEMRIS in the order (type , X, Y, Z)
A=permute(A,[4 1 2 3]); 

SF=[pwd,'/sample.h5'];
if exist(SF)==2, delete(SF); end
h5create(SF,'/sample/data',size(A));
h5create(SF,'/sample/resolution',[1 3]);
h5create(SF,'/sample/offset',[1 3]);
h5write(SF,'/sample/data', A);
h5write(SF,'/sample/resolution',Sample.RES);
h5write(SF,'/sample/offset',Sample.OFFSET);


maxM0 = max(Sample.M0(:));

return;

%%%
function Sample=getShape(VA)
%try
        if length(VA)==11, fname=VA{11}; end
        dim    = VA{2};
        A      = zeros(max(dim));
        [nx,ny,nz]=size(A);
        [I,J,K]=meshgrid(1:nx,1:ny,1:nz); 
        M0=1;T1=1000;T2=1000;DB=0;NN=0;
        S={'M0','T1','T2','T2S','DB','NN'};
        switch(lower(VA{1}))
            case '2d sphere'
                R=dim(1)/2;
                if nz>1
                    L=find((I-R).^2+(J-R).^2+(K-R).^2 < R^2);
                else
                    L=find((I-R).^2+(J-R).^2 < R^2);
                end
                A(L)=1;
                for i=1:6
                 if length(VA)>3+i
                 eval([S{i},'=VA{4+i}(1);']);
                 end
                 eval([S{i},'=',S{i},'*A;']);
                end
               if length(VA)<11, fname=sprintf('sphere_%d.bin',length(L)); end
            case '1d column'
                dim = dim/2;
                res = VA{3}; if numel(res)==1 && res ~=1; dim(2)=round(dim(2)*res); end
                A=zeros(dim(1)*dim(2),11);
                [nx,ny,nz]=size(A);
                [I,J,K]=meshgrid(1:nx,1:ny,1:nz); 
                %index of odd and even blocks        
                Lo=[]; Le=[]; II=[1:dim(1)]';
                for i=1:dim(2)
                    if mod(i,2)
                        Lo = [Lo ; [II]+(i-1)*dim(1)];
                    else
                        Le = [Le ; [II]+(i-1)*dim(1)];
                    end
                end
                A(Lo,6)=1;
            
                for i=1:6
                 if length(VA)>3+i
                 eval([S{i},'=VA{4+i}(1);']);
                 end
                 eval([S{i},'=',S{i},'*A;']);
                 s=VA{4+i}(min([2 length(VA{4+i})]));
                 eval([S{i},'(Le,6)=s;']);
                end
            case '2d 2-spheres'
                %outer sphere
                R=max(dim)/2;
                if nz>1
                    L=find((I-R).^2+(J-R).^2+(K-R).^2 < R^2);
                else
                    L=find((I-R).^2+(J-R).^2 < R^2);
                end
                A(L)=1;
                for i=1:6
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
                for i=1:6
                 if length(VA)>3+i
                  s=VA{4+i}(min([2 length(VA{4+i})]));
                  eval([S{i},'(L)=s;']);
                 end
                end
                if length(VA)<11, fname=sprintf('sphere_%d.bin',length(L)); end
            case 'shepp-logan'
                A=fliplr(phantom(round(dim(1)/2))');
                A(A<eps)=0;
                for i=1:6
                 if length(VA)>3+i
                 eval([S{i},'=VA{4+i}(1);']);
                 end
                 if i>1
                    eval([S{i},'=',S{i},'*A;']);
                 else
                    eval([S{i},'=A.^',S{i},';']);
                 end
                end

            otherwise
                error('unknown pre-defined sample string')
        end

        res    = VA{3};
        offset = VA{4};
    %catch
    %    error('input is neither a sample-structure, nor a pre-defined sample')
    %end
 
    Sample=struct('M0',M0,'T1',T1,'T2',T2,'T2S',T2S,'DB',DB,'NN',NN,'RES',res,'OFFSET',offset,'FNAME',fname);

return;
