function outFT = ft(data)

% forward FT in one dimension

% 1D fft on input data, all channels; complex output

checkSize = size(data);
nDim      = length(checkSize);
dim1      = size(data,1);

if(dim1 == 1 && nDim == 2) %this is a vector
    outIFT = fftshift(fft(fftshift(data))); %simple 1D transform for vector
end;

% if(nDim == 3) %assume we have slices, rows, channels.
%     [nSlices,nRows,nCh] = size(data);
%     outIFT              = zeros(nSlices,nCols,nCh);
% end;  
    
% manage full 3D data set, fft in slice direction    
if(nDim == 4) %assume we have 3D with slices, rows, cols, channels; want ft in slice direction
    [nSlices,nRows,nCols,nCh] = size(data);
    outFT                     = zeros(nSlices,nRows,nCols,nCh);
    rawBuf                    = zeros(nSlices,nCols);
    ftBuf                     = zeros(nSlices,nCols); 
    for(row = 1:nRows)
        for(channel = 1:nCh)
            rawBuf(:,:)            = data(:,row,:,channel);
            ftBuf                  = fftshift(fft(fftshift(rawBuf))); % ft in slice direction for all columns (most efficient)
            outFT(:,row,:,channel) = ftBuf(:,:);
        end;
    end;
end;
    
    
    
