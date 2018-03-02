function cpxImgs = ift3_HMC(HMCdata)

% (NOTE: HMC 3D "raw data" has already had ifft done in slice direction, so ift3 will not work on it.)
% all we do here is ifft2 for all slices (like a 2D recon)

[nSlices,nRows,nCols,nCh] = size(HMCdata);
cpxImgs                   = zeros(nSlices,nRows,nCols,nCh);

% do 2D ifft for all slices, channels
planeBuf = zeros(nRows,nCols,nCh);
for(slice = 1:nSlices)
    planeBuf(:,:,:)       = HMCdata(slice,:,:,:);
    sliceBuf              = ift2(planeBuf);
    cpxImgs(slice,:,:,:)  = sliceBuf(:,:,:);
end;