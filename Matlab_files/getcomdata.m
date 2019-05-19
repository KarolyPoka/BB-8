close all;
% change the COM Port number as needed
serialPort= serial('COM3','baudrate',115200); 
fopen(serialPort);
%flushinput(serialPort);
disp('Waiting for 150 samples...');
try
    
        u=[];
        y=[];
        for (k=1:150)
            values=fscanf(serialPort,'%f%f');
            u=[u;values(1)];
            y=[y;values(2)];
        end
        fclose(serialPort);
    
catch
    fclose(serialPort);
    error('Something went wrong.');
end
disp('... ready!');
disp('Samples of speed saved to vector y, samples of control signal saved to vector u.');
figure(1);
plot(1:length(y),y);
title('Speed [rad/s]');
figure(2);
plot(1:length(u),u);
title('Control signal (voltage - 0...5)');

