plotResult("./matlab/lossy/airplane",1);
plotResult("./matlab/lossy/anemone",2);
plotResult("./matlab/lossy/arial",3);
plotResult("./matlab/lossy/baboon",4);
plotResult("./matlab/lossy/bike3",5);
plotResult("./matlab/lossy/boat",6);
plotResult("./matlab/lossy/girl",7);
plotResult("./matlab/lossy/house",8);
plotResult("./matlab/lossy/lena",9);
plotResult("./matlab/lossy/monarch",10);
plotResult("./matlab/lossy/peppers",11);
plotResult("./matlab/lossy/tulips",12);

function e = plotResult(dir,fig)
    
    formatSpec = '%d';

    fileAxis = fopen(dir + '/xAxis.txt','r');
    axis = fscanf(fileAxis,formatSpec);

    fileY = fopen(dir + '/YFrequency.txt','r');
    y = fscanf(fileY,formatSpec);
    
    fileU = fopen(dir + '/UFrequency.txt','r');
    u = fscanf(fileU,formatSpec);
    
    fileV = fopen(dir + '/VFrequency.txt','r');
    v = fscanf(fileV,formatSpec);

    figure(fig);
    plot(axis, y, 'b', axis, u, 'r', axis, v, 'g');
    legend("Y","U", "V")
    xlabel("Residuals");
    xlim([-255,255]);
    grid on;
end