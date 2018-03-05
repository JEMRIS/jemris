function ReconJemrisCartesianDataMain

% reconstructs data you just acquired.

clear all
addpath './matlab/ReconUtilities/';



%PARAMETERS--------------------
VENC=100; %mm/s
invertVel=+1;   %To invert velocity sign

NdispRows=4;
NdispCols=4;
NsubPlots=NdispRows*NdispCols;



% set up paths, hopefully easy to change as needed.
basePathStart = '.'; %change to fit your own needs
localDir      = '';                       % local Jemris work area
basePath      = strcat([basePathStart,localDir]);      % make full path to here
params.path      = uigetdir(strcat([basePath]));


% flags to adjust actions.
control.saveJPG     = false; % option to save image in JPG folder
control.isJemris27  = true; %identifying software version



    %Calculate image dimensions
    params.seq        = 'name';
    %Read image size
    t=h5read(strcat([params.path,'/signals.h5']),'/signal/times');
    Dt=t(2)-t(1);
    N=1; while(t(N+1)-t(N)<1.1*Dt)  N=N+1; end;
    params.nX         = N;
    params.nY         = N;  %SET NUMBER OF PHASE ENCODING STEPS IF DIFFERENT FROM READOUT
    params.nZ         = size(t,1)/(params.nX*params.nY*2);
    %Read channels number
    I=h5info(strcat([params.path,'/signals.h5']),'/signal/channels');
    params.channels   = size(I.Datasets,1);
    % **** end adjust for each seq ***
    params.nRows      = params.nY;
    params.nCols      = params.nX;
    params.nSli       = params.nZ*2;
    control.fullTitle = false;
    disp('Channels: ');disp(params.channels);
    disp('Matrix size (slices): ');disp(params.nSli);
    disp('Matrix size (readout): ');disp(params.nRows);
    disp('Matrix size (phase): ');disp(params.nCols);




params.dataName   = 'signals'; %data file name for Jemris 2.7
params.fileName = strcat([params.path,'/',params.dataName]); 
rawFileName = params.fileName;

% read the data, organize into complex matrix
params.fileName = strcat([rawFileName,'.h5']);
rawData         = ReadCartesianData_PhaseContrast(params,control);
params.nSli     = params.nZ;
rawDataP = zeros(params.nY,params.nX,params.nZ);
rawDataN = zeros(params.nY,params.nX,params.nZ);
for i=0:params.nZ-1
    rawDataP(:,:,i+1)=rawData(:,:,i*2+1);
    rawDataN(:,:,i+1)=rawData(:,:,i*2+2);
end;


 
% Cartesian recon
imageP = ReconCartesianData(sum(rawDataP,4),params,control);
imageN = ReconCartesianData(sum(rawDataN,4),params,control);
    






%Phase image-------------------------------------------------

imagePhase=angle(imageN./imageP);
imagePhase=VENC*imagePhase/pi*invertVel; %Donne vitesses en mm/s pour division cplexe

imDispl=imagePhase;
sub=0;
figure;
for i=1:params.nZ
  sub=sub+1;
  if sub>NsubPlots
      figure;
      sub=1;
  end;   
  subplot(NdispRows,NdispCols,sub);
  %colormap(gray);
  imagesc(imDispl(:,:,i),[-VENC VENC]);%,[0 8.5e-7]);%,[-27 -14]);%,[0 20e-7]);
end;
set(gcf,'NextPlot','add');
axes;
h = title('VELOCITY MAP','FontSize',12');
set(gca,'Visible','off');
set(h,'Visible','on'); 


if(control.saveJPG)
    saveas(fh1,strcat([params.path,'/PhaseImage']),'jpg');
end;



%Magnitude image (complex subtraction)----------------------------------------------------

imageMagn=abs(imageP-imageN);

imDispl=imageMagn;
maxScale=max(max(max(imDispl)));
minScale=min(min(min(imDispl)));
maxScale(maxScale<=minScale)=minScale+1;
sub=0;
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
set(gcf,'NextPlot','add');
axes;
h = title('COMPLEX SIGNAL DIFFERENCE','FontSize',12');
set(gca,'Visible','off');
set(h,'Visible','on');




%Phase * Magnitude--------------------------------------------
imageMagn=abs(imageP).*abs(angle(imageN./imageP));

imDispl=imageMagn;
maxScale=max(max(max(imDispl)));
minScale=min(min(min(imDispl)));
maxScale(maxScale<=minScale)=minScale+1;
sub=0;
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
set(gcf,'NextPlot','add');
axes;
h = title('MAGNITUDE x PHASE','FontSize',12');
set(gca,'Visible','off');
set(h,'Visible','on'); 






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

    
  
