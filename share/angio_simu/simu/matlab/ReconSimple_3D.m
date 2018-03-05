function ReconJemrisCartesianDataMain

% reconstructs data you just acquired.

clear all
addpath './matlab/ReconUtilities/';



% set up paths, hopefully easy to change as needed.
basePathStart = '.'; %change to fit your own needs
localDir      = '';                       % local Jemris work area
basePath      = strcat([basePathStart,localDir]);      % make full path to here
params.path      = uigetdir(strcat([basePath]));


% flags to adjust actions.
control.saveJPG     = false; % option to save image in JPG folder
control.isJemris27  = true; %identifying software version


%OPTIONS (boolean)
video=1;
videoSave=0;
niftiSave=0;
MIPdisp=1;
mask=0; 
allDispl=0;
rawDispl=0;


%PARAMETERS
slowMovie=4;
LinesDisp=4;
NdispRows=LinesDisp;
NdispCols=LinesDisp;
NsubPlots=NdispRows*NdispCols;
thresold=10; %Min signal coefficient for optional mask



    %Calculate image dimensions
    params.seq        = 'name';
    %Read image size
    t=h5read(strcat([params.path,'/signals.h5']),'/signal/times');
    Dt=t(2)-t(1);
    N=1; while(t(N+1)-t(N)<1.1*Dt)  N=N+1; end;
    params.nX         = N;
    params.nY         = N;  %SET NUMBER OF PHASE ENCODING STEPS IF DIFFERENT FROM READOUT
    params.nZ         = size(t,1)/(params.nX*params.nY);
    %Read channels number
    I=h5info(strcat([params.path,'/signals.h5']),'/signal/channels');
    params.channels   = size(I.Datasets,1);
    % **** end adjust for each seq ***
    params.nRows      = params.nY;
    params.nCols      = params.nX;
    params.nSli       = params.nZ;
    control.fullTitle = false;
    disp('Channels: ');disp(params.channels);
    disp('Matrix size (slices): ');disp(params.nSli);
    disp('Matrix size (readout): ');disp(params.nRows);
    disp('Matrix size (phase): ');disp(params.nCols);




    
%Data file name for Jemris 2.7
params.dataName   = 'signals'; 
params.fileName = strcat([params.path,'/',params.dataName]); 
rawFileName = params.fileName;

%Read the dataset, organize into complex matrix
params.fileName = strcat([rawFileName,'.h5']);
rawData              = ReadCartesianData_PhaseContrast(params,control);
params.nSli       = params.nZ;



%Cartesian recon from all coils channels
image = zeros(params.nY,params.nX,params.nZ);
image = ReconCartesianData(sum(rawData,4),params,control);



%---IMAGES---
%Magn and phase images calculation
%Morphological tissues
kspace= log(abs(sum(rawData,4)));
kphase= angle(sum(rawData,4));
magn  = abs(image);
phase = angle(image);


%Mask calculation (abort areas with too low signal value)
masque=abs(image);
masqueSum=sum(sum(sum(masque)));
maskThresold=thresold*masqueSum/(params.nX*params.nY*params.nZ);
masque(masque<maskThresold)=0;
masque(masque>=maskThresold)=1;


%Raw data
if(rawDispl)
    maxScale=max(max(max(kspace)));
    minScale=min(min(min(kspace)));
    maxScale(maxScale<=minScale)=minScale+1;
    figure;
    sub=0;
    for i=1:params.nZ
        sub=sub+1;
        if sub>NsubPlots
            figure;
            sub=1;
        end;
        subplot(NdispRows,NdispCols,sub);
        colormap(gray);
        imagesc(kspace(:,:,i),[minScale maxScale]);
    end;
end;


%3D images display (choose which image to display, from ---IMAGES---)
imDispl=magn;
if(mask)
    imDispl=imDispl.*masque;
end;
disp(size(imDispl));
maxScale=max(max(max(imDispl)));
minScale=min(min(min(imDispl)));
maxScale(maxScale<=minScale)=minScale+1;
sub=0;
if(allDispl)
  figure;
  for i=1:params.nZ
    sub=sub+1;
    if sub>NsubPlots
        figure;
        sub=1;
    end;
    subplot(NdispRows,NdispCols,sub);
    colormap(gray);
    imagesc(imDispl(:,:,i),[minScale maxScale]);%,[0 8.5e-7]);%,[-27 -14]);%,[0 20e-7]);
  end;
end;
%Save images as nifti
if(niftiSave)
    nii=make_nii(imDispl);
    save_nii(nii,niftiFile);
end;
%Save images as 3D movie
if(video)
figure;
for i=1:params.nZ
    colormap(gray);
    imagesc(imDispl(:,:,i),[minScale maxScale]);
    for slow=slowMovie:-1:1
        frames(slowMovie*i-slow+1)=getframe;
    end;
end;
close;
if(videoSave)
    vid = VideoWriter('ima3D.avi');
    open(vid);
    writeVideo(vid,frames);
    close(vid);
end;
end;










% *****************************************
% **** Manage Cartesian Reconstructions ***
% *****************************************
function image = ReconCartesianData(rawData,params,control)


    
    % straight Cartesian recon
    image = ift3(rawData);

    
  
