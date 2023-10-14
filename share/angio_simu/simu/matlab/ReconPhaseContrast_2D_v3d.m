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
video=0;
videoSave=0;
niftiSave=0;
MIPdisp=1;
mask=0; 
allDispl=1;
rawDispl=0;


%PARAMETERS
VENC=100; %mm/s
invertVel=1; %To invert velocity sign
slowMovie=4;
LinesDisp=4;
NdispRows=LinesDisp;
NdispCols=LinesDisp;
NsubPlots=NdispRows*NdispCols;
thresold=20; %Min signal coefficient for optional mask



    %Calculate image dimensions
    params.seq        = 'name';
    %Read image size
    t=h5read(strcat([params.path,'/signals.h5']),'/signal/times');
    Dt=t(2)-t(1);
    N=1; while(t(N+1)-t(N)<1.1*Dt)  N=N+1; end;
    params.nX         = N;
    params.nY         = N;  %SET NUMBER OF PHASE ENCODING STEPS IF DIFFERENT FROM READOUT
    params.nZ         = size(t,1)/(params.nX*params.nY*4);
    %Read channels number
    I=h5info(strcat([params.path,'/signals.h5']),'/signal/channels');
    params.channels   = size(I.Datasets,1);
    % **** end adjust for each seq ***
    params.nRows      = params.nY;
    params.nCols      = params.nX;
    params.nSli       = params.nZ*4;
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
rawData0 = zeros(params.nY,params.nX,params.nZ);
rawDataX = zeros(params.nY,params.nX,params.nZ);
rawDataY = zeros(params.nY,params.nX,params.nZ);
rawDataZ = zeros(params.nY,params.nX,params.nZ);
for i=0:params.nZ-1
    rawData0(:,:,i+1)=rawData(:,:,i*4+1);
    rawDataX(:,:,i+1)=rawData(:,:,i*4+2);
    rawDataY(:,:,i+1)=rawData(:,:,i*4+3);
    rawDataZ(:,:,i+1)=rawData(:,:,i*4+4);
end;


%Cartesian recon from all coils channels
image0 = ReconCartesianData(sum(rawData0,4),params,control);
imageX = ReconCartesianData(sum(rawDataX,4),params,control);
imageY = ReconCartesianData(sum(rawDataY,4),params,control);
imageZ = ReconCartesianData(sum(rawDataZ,4),params,control);


%---IMAGES---
%Magn and phase images calculation
%Morphological tissues
rephased=abs(image0);
kspace= log(abs(sum(rawData,4)));
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
phX_x_magn = phX.*rephased;
phY_x_magn = phY.*rephased;
phZ_x_magn = phZ.*rephased;
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


% X velocity Slices display (choose which image to display, from ---IMAGES---)
imDispl=Vx;
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
set(gcf,'NextPlot','add');
axes;
h = title('X DIRECTION','FontSize',12');
set(gca,'Visible','off');
set(h,'Visible','on'); 

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

% Y velocity Slices display (choose which image to display, from ---IMAGES---)
imDispl=Vy;
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
set(gcf,'NextPlot','add');
axes;
h = title('Y DIRECTION','FontSize',12');
set(gca,'Visible','off');
set(h,'Visible','on'); 
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

% Z velocity Slices display (choose which image to display, from ---IMAGES---)
imDispl=Vz;
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
set(gcf,'NextPlot','add');
axes;
h = title('Z DIRECTION','FontSize',12');
set(gca,'Visible','off');
set(h,'Visible','on'); 
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









% *****************************************
% **** Manage Cartesian Reconstructions ***
% *****************************************
function image = ReconCartesianData(rawData,params,control)

    % Flip even numbered data lines for epi
    if(strcmp('EPI',params.seq))
        rawData = EpiFlipData(rawData);
    end;

    % shuffle data lines for centric ordered FSE
    if( strcmp(params.seq,'FSE') && strcmp(params.order,'centric') )
        rawData = ReorderFSEcentric(rawData,params);  
    end;

    % straight Cartesian recon
    for i=1:params.nZ
        image(:,:,i)   = fliplr(conj(ift2(rawData(:,:,i))));
    end;
    
  
