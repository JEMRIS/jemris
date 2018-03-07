function outImgComplex = ift2(data)

% 2D ifft on input data, all channels; complex output

checkSize = size(data);
nDim      = length(checkSize);

if(nDim == 3)
    [nRows,nCols,nCh] = size(data);
    outImgComplex     = zeros(nRows,nCols,nCh);
end;
if(nDim == 2)
    [nRows,nCols]     = size(data);
    nCh               = 1;
    outImgComplex     = zeros(nRows,nCols);
end;

if( nDim < 2 || nDim > 3)
    disp(['error in ift2:  ', 'data size error: can deal with [rows,cols] or [rows, cols, channels] only']);
    errorStop_ift2
end;
 

inBuffer      = zeros(nRows,nCols);
outBuffer     = zeros(nRows,nCols);


if(nDim == 2)
    inBuffer                       = data;
    outBuffer                      = ifftshift(ifft2(ifftshift(inBuffer)));
    outImgComplex                  = outBuffer;
else
    for(channel = 1:nCh)
        inBuffer(:,:)              = data(:,:,channel);
        outBuffer                  = ifftshift(ifft2(ifftshift(inBuffer)));
        outImgComplex(:,:,channel) = outBuffer(:,:);
    end;
end;
