plotResult("./matlab/lossless/airplane",1);
plotResult("./matlab/lossless/anemone",2);
plotResult("./matlab/lossless/arial",3);
plotResult("./matlab/lossless/baboon",4);
plotResult("./matlab/lossless/bike3",5);
plotResult("./matlab/lossless/boat",6);
plotResult("./matlab/lossless/girl",7);
plotResult("./matlab/lossless/house",8);
plotResult("./matlab/lossless/lena",9);
plotResult("./matlab/lossless/monarch",10);
plotResult("./matlab/lossless/peppers",11);
plotResult("./matlab/lossless/tulips",12);

function e = plotResult(dir,fig)
    
    formatSpec = '%d';

    fileAxis = fopen(dir + '/xAxis.txt','r');
    axis = fscanf(fileAxis,formatSpec);

    fileY = fopen(dir + '/YFrequence.txt','r');
    y = fscanf(fileY,formatSpec);
    
    fileU = fopen(dir + '/UFrequence.txt','r');
    u = fscanf(fileU,formatSpec);
    
    fileV = fopen(dir + '/VFrequence.txt','r');
    v = fscanf(fileV,formatSpec);

    figure(fig);
    plot(axis, y, 'b', axis, u, 'r', axis, v, 'g');
    legend("Y","U", "V")
    xlabel("Residuals");
    grid on;
end