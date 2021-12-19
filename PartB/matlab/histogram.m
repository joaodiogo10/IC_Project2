plotResult("sample01");
plotResult("sample02");
plotResult("sample03");
plotResult("sample04");
plotResult("sample05");
plotResult("sample06");
plotResult("sample07");


function e = plotResult(name)
    formatSpec = '%d';
    fileChannel1 = fopen(name + 'Matlab/FirstOrderCodec/channel1.txt','r');
    channel1 = fscanf(fileChannel1,formatSpec);
    
    fileChannel2= fopen(name + 'Matlab/FirstOrderCodec/channel2.txt','r');
    channel2 = fscanf(fileChannel2,formatSpec);

    f = figure('visible','off');
    xAxisCh1=(-length(channel1)/2+1):(length(channel1)/2);
    xAxisCh2=(-length(channel2)/2+1):(length(channel2)/2);
    plot(xAxisCh1, channel1, 'b', xAxisCh2, channel2, 'r');
    legend("Channel 1", "Channel 2")
    xlabel("Amplitude");
    grid on;
    saveas(f, name + 'Matlab/FirstOrderCodec/' + name + '.png')

    formatSpec = '%d';
    fileChannel1 = fopen(name + 'Matlab/PolynomialCodec/channel1.txt','r');
    channel1 = fscanf(fileChannel1,formatSpec);
    
    fileChannel2= fopen(name + 'Matlab/PolynomialCodec/channel2.txt','r');
    channel2 = fscanf(fileChannel2,formatSpec);

    f = figure('visible','off');
    xAxisCh1=(-length(channel1)/2+1):(length(channel1)/2);
    xAxisCh2=(-length(channel2)/2+1):(length(channel2)/2);
    plot(xAxisCh1, channel1, 'b', xAxisCh2, channel2, 'r');
    legend("Channel 1", "Channel 2")
    xlabel("Amplitude");
    grid on;
    saveas(f,name + 'Matlab/PolynomialCodec/' + name + '.png')

    fileChannel2= fopen(name + 'Matlab/RedundancyCodec/channel2.txt','r');
    channel2 = fscanf(fileChannel2,formatSpec);

    f = figure('visible','off');
    xAxisCh1=(-length(channel1)/2+1):(length(channel1)/2);
    xAxisCh2=(-length(channel2)/2+1):(length(channel2)/2);
    plot(xAxisCh1, channel1, 'b', xAxisCh2, channel2, 'r');
    legend("Channel 1", "Channel 2")
    xlabel("Amplitude");
    grid on;
    saveas(f,name + 'Matlab/RedundancyCodec/' + name + '.png')
end