function cpxImgs = ift3(trueRawData)

% do 3D ifft for 3D true raw data set, all channels

% (NOTE: HMC 3D "raw data" has already had ifft done in slice direction, so this will not work on it.)


[nSlices,nRows,nCols,nCh] = size(trueRawData);
cpxImgs                   = zeros(nSlices,nRows,nCols,nCh);

% ifft in slice direction, all rows, cols, channels
outIFT                    = ift(trueRawData); 

% do 2D ifft for all slices, channels
planeBuf = zeros(nRows,nCols,nCh);
for(slice = 1:nSlices)
    planeBuf(:,:,:)       = outIFT(slice,:,:,:);
    sliceBuf              = ift2(planeBuf);
    cpxImgs(slice,:,:,:)  = sliceBuf(:,:,:);
end;
