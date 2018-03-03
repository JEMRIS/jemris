function complexData = ReadCartesianData(params,control)


if(control.isJemris27)


    nRows = params.nRows;
    nCols = params.nCols;
    nSli  = params.nSli ;

    rawFileName1 = params.fileName;
 %   rawFileName2 = strcat([params.fileName,'N.h5']);
    
    % code borrowed from JEMRIS_sim.m
    t1           = h5read (rawFileName1, sprintf('/signal/times'));%disp('times');disp(t1);disp('times');
    [t1,I]       = sort(t1);

    
    for i=1:params.channels
        A1        = ( h5read (rawFileName1, sprintf('/signal/channels/%02i',i-1)) )';
        M1(:,:,i) = A1(I,:);
    
    	n               = size(M1,1);
    	myR1(1:n)        = M1(1:n,1,i); %real line
    	myI1(1:n)        = M1(1:n,2,i); %imag line
	%disp('myI1');disp(myI1);disp('myI1');
    
       
    	% reorganize into a matrix of complex numbers
    	aaR1             = reshape(myR1,nCols,nRows,nSli);
    	aaI1             = reshape(myI1,nCols,nRows,nSli);

    
    	%If Magnitude Image:
    	for sli=1:nSli
     	    complexData(:,:,sli,i)     = rot90(complex(aaR1(:,:,sli),aaI1(:,:,sli)),-1); %rotate clockwise 90 degrees
	    complexData(:,:,sli,i) = fliplr(complexData(:,:,sli,i));			 %Flip matrix left-right (display with time right orientation)
    	end

	
    	%Invert phase sign
    	%complexData	= conj(complexData);
    	%disp('complexData');disp(complexData);disp('complexData');%figure;imagesc(angle(complexData(:,:)));colormap(gray);
    
    end;
 
    
else
    
    nRows            = params.nRows;
    nCols            = params.nCols;
    rawFileName      = params.fileName;
    
    numBytesPerPoint = 8; %doubles
    multiplier       = 4; % timeNumber, dataReal, dataImag, otherNumber

    % read the binary data file
    f               = fopen(rawFileName,'r','l'); % Kaveh built the S/W for windows, so we do not have to byte swap.
    A               = fread(f,Inf,'double','l'); %read all the data as a vector of doubles
    fclose(f);

    % reshape it into 4 very long columns
    n               = size(A,1)/4; 
    A               = reshape(A,4,n)';

    % get all the real numbers into vector myR, all imaginaries into vector myI 
    myR(1:n)        = A(1:n,2); %real line
    myI(1:n)        = A(1:n,3); %imag line

    % reorganize into a matrix of complex numbers
    aaR             = reshape(myR,nCols,nRows);
    aaI             = reshape(myI,nCols,nRows);
    complexData     = rot90(complex(aaR,aaI),-1); %rotate clockwise 90 degrees
end;