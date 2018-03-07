function outDataComplex = ft2(imgIn)

% 2D fft on input complex images, all channels; complex output

checkSize = size(imgIn);
nDim      = length(checkSize);

if(nDim == 3)
    [nRows,nCols,nCh] = size(imgIn);
    outDataComplex    = zeros(nRows,nCols,nCh);
end;
if(nDim == 2)
    [nRows,nCols]     = size(imgIn);
    nCh               = 1;
    outDataComplex    = zeros(nRows,nCols);
end;

if( nDim < 2 || nDim > 3)
    disp(['error in ft2:  ', 'data size error: can deal with [rows,cols] or [rows, cols, channels] only']);
    errorStop_ft2
end;
 

inBuffer      = zeros(nRows,nCols);
outBuffer     = zeros(nRows,nCols);


if(nDim == 2)
    inBuffer                       = imgIn;
    outBuffer                      = fftshift(fft2(fftshift(inBuffer)));
    outDataComplex                 = outBuffer;
else
    for(channel = 1:nCh)
        inBuffer(:,:)              = imgIn(:,:,channel);
        outBuffer                  = fftshift(fft2(fftshift(inBuffer)));
        outDataComplex(:,:,channel)= outBuffer(:,:);
    end;
end;



