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
mask=1; 
allDispl=0;
rawDispl=0;


%PARAMETERS
VENC=150; %mm/s
invertVel=1; %To invert velocity sign
slowMovie=4;
LinesDisp=4;
NdispRows=LinesDisp;
NdispCols=LinesDisp;
NsubPlots=NdispRows*NdispCols;
thresold=10; %Min signal coefficient for optional mask



    %Calculate image dimensions
    params.seq        = 'name';
    %Read image size
    t=h5read(strcat([params.path,'/signals0.h5']),'/signal/times');
    Dt=t(2)-t(1);
    N=1; while(t(N+1)-t(N)<1.1*Dt)  N=N+1; end;
    params.nX         = N;
    params.nY         = N;  %SET NUMBER OF PHASE ENCODING STEPS IF DIFFERENT FROM READOUT
    params.nZ         = size(t,1)/(params.nX*params.nY);
    %Read channels number
    I=h5info(strcat([params.path,'/signals0.h5']),'/signal/channels');
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

%Read the 4 subsequences signals, organize into complex matrix
params.fileName = strcat([rawFileName,'0.h5']);
rawData0              = ReadCartesianData_PhaseContrast(params,control);
params.fileName = strcat([rawFileName,'X.h5']);
rawDataX              = ReadCartesianData_PhaseContrast(params,control);
params.fileName = strcat([rawFileName,'Y.h5']);
rawDataY              = ReadCartesianData_PhaseContrast(params,control);
params.fileName = strcat([rawFileName,'Z.h5']);
rawDataZ              = ReadCartesianData_PhaseContrast(params,control);



%Cartesian recon from all coils channels
image0 = ReconCartesianData(sum(rawData0,4),params,control);
imageX = ReconCartesianData(sum(rawDataX,4),params,control);
imageY = ReconCartesianData(sum(rawDataY,4),params,control);
imageZ = ReconCartesianData(sum(rawDataZ,4),params,control);



%---IMAGES---
%Magn and phase images calculation
%Morphological tissues
rephased=abs(image0);
kspace= log(abs(sum(rawData0,4)));
%Morphological flow images
magnX = abs(image0-imageX);
magnY = abs(image0-imageY);
magnZ = abs(image0-imageZ);
SumMagn = sqrt(magnX.*magnX + magnY.*magnY + magnZ.*magnZ);
%Phase and velocity images
phX = angle(image0./imageX);
phY = angle(image0./imageY);
phZ = angle(image0./imageZ);
phase = sqrt(phX.^2 + phY.^2 + phZ.^2);
Vx=phX*VENC/pi*invertVel;
Vy=phY*VENC/pi*invertVel;
Vz=phZ*VENC/pi*invertVel;
V=VENC*phase/pi; %mm/s, set VENC correctly in the parameters
%Morphological flow image
phase_x_magn = phase.*rephased;


%Mask calculation (abort areas with too low signal value)
masque=abs(image0);
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
imDispl=SumMagn;
if(mask)
    imDispl=imDispl.*masque;
end;
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
    %colormap(gray);
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


%MIP
if(MIPdisp)
for i=1:3
figure;
colormap(gray);
MIPimage=abs(imDispl);
MIP=max(MIPimage,[],i);
imagesc(squeeze(MIP),[minScale maxScale]);
xlabel('readout (pixels)','FontSize',12);
ylabel('phase (pixels)','FontSize',12);
title('MIP','FontSize',12');
colorbar;
end;
end;








% *****************************************
% **** Manage Cartesian Reconstructions ***
% *****************************************
function image = ReconCartesianData(rawData,params,control)


    
    % straight Cartesian recon
    image = ift3(rawData);

    
  
