function outIFT = ift(data)

% inverse FFT in one dimension

% 1D ifft on input data, all channels; complex output
%disp(data);
checkSize = size(data);disp(checkSize);
nDim      = length(checkSize);
dim1      = size(data,1);

if(dim1 == 1 && nDim == 2) %this is a vector
    outIFT = ifftshift(ifft(ifftshift(data))); %simple 1D transform for vector
end;

% if(nDim == 3) %assume we have slices, rows, channels.
%     [nSlices,nRows,nCh] = size(data);
%     outIFT              = zeros(nSlices,nCols,nCh);
% end;  
    
% manage full 3D data set, ifft in slice direction    
if(nDim == 4 || nDim==3) %assume we have 3D with slices, rows, cols, channels; want ift in slice direction
    [nRows,nCols,nSlices,nCh] = size(data);disp([nRows,nCols,nSlices,nCh]);
    outIFT = zeros(nRows,nCols,nSlices,nCh);
    rawBuf = zeros(nCols,nSlices);
    iftBuf = zeros(nCols,nSlices); 
    for(row = 1:nRows)
        for(channel = 1:nCh)
            rawBuf(:,:)             = data(:,row,:,channel);
            iftBuf                  = ifftshift(ifft(ifftshift(rawBuf))); % ift in slice direction for all columns (most efficient)
            outIFT(:,row,:,channel) = iftBuf(:,:);
        end;
    end;
end;
    
    
    
