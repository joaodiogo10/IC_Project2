plotResult("sample01", "4");
plotResult("sample02", "4");
plotResult("sample03", "4");
plotResult("sample04", "4");
plotResult("sample05", "4");
plotResult("sample06", "4");
plotResult("sample07", "4");
plotResult("sample01", "10");
plotResult("sample02", "10");
plotResult("sample03", "10");
plotResult("sample04", "10");
plotResult("sample05", "10");
plotResult("sample06", "10");
plotResult("sample07", "10");

function e = plotResult(name, nBits)
    formatSpec = '%d';
    fileChannel1 = fopen(nBits + '_bits/' + name + 'Matlab/FirstOrderCodec/channel1.txt','r');
    channel1 = fscanf(fileChannel1,formatSpec);
    
    fileChannel2= fopen(nBits + '_bits/' + name + 'Matlab/FirstOrderCodec/channel2.txt','r');
    channel2 = fscanf(fileChannel2,formatSpec);

    f = figure('visible','off');
    xAxisCh1 = -1000:1000;
    xAxisCh2 = -1000:1000;
    channel1 = channel1(fix((size(channel1,1)/2)-1000):(fix(size(channel1,1)/2)+1000));
    channel2 = channel2(fix((size(channel2,1)/2)-1000):(fix(size(channel2,1)/2)+1000));
    plot(xAxisCh1, channel1, 'b', xAxisCh2, channel2, 'r');
    legend("Channel 1", "Channel 2")
    xlabel("Residuais");
    grid on;
    saveas(f, "../resultsLossy/"  + nBits + '_bits/' + name + 'Results/FirstOrderCodec/' + name + '.png')

    formatSpec = '%d';
    fileChannel1 = fopen(nBits + '_bits/' + name + 'Matlab/PolynomialCodec/channel1.txt','r');
    channel1 = fscanf(fileChannel1,formatSpec);
    
    fileChannel2= fopen(nBits + '_bits/' + name + 'Matlab/PolynomialCodec/channel2.txt','r');
    channel2 = fscanf(fileChannel2,formatSpec);

    f = figure('visible','off');
    xAxisCh1 = -1000:1000;
    xAxisCh2 = -1000:1000;
    channel1 = channel1(fix((size(channel1,1)/2)-1000):(fix(size(channel1,1)/2)+1000));
    channel2 = channel2(fix((size(channel2,1)/2)-1000):(fix(size(channel2,1)/2)+1000));
    plot(xAxisCh1, channel1, 'b', xAxisCh2, channel2, 'r');
    legend("Channel 1", "Channel 2")
    xlabel("Residuais");
    grid on;
    saveas(f, "../resultsLossy/" + nBits + '_bits/' + name + 'Results/PolynomialCodec/' + name + '.png')

    fileChannel1= fopen(nBits + '_bits/' + name + 'Matlab/RedundancyCodec/channel2.txt','r');
    channel1 = fscanf(fileChannel1,formatSpec);

    fileChannel2= fopen(nBits + '_bits/' + name + 'Matlab/RedundancyCodec/channel2.txt','r');
    channel2 = fscanf(fileChannel2,formatSpec);

    f = figure('visible','off');
    xAxisCh1 = -1000:1000;
    xAxisCh2 = -1000:1000;
    channel1 = channel1(fix((size(channel1,1)/2)-1000):(fix(size(channel1,1)/2)+1000));
    channel2 = channel2(fix((size(channel2,1)/2)-1000):(fix(size(channel2,1)/2)+1000));
    plot(xAxisCh1, channel1, 'b', xAxisCh2, channel2, 'r');
    legend("Channel 1", "Channel 2")
    xlabel("Residuais");
    grid on;
    saveas(f, "../resultsLossy/"  + nBits + '_bits/'  + name + 'Results/RedundancyCodec/' + name + '.png')
end