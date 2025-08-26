clc; clear; close all;

% Audio Files 
file_wav  = "Beethoven_5th_Symphony.wav"; 
file_opus = "resultados/Beethoven_5th_Symphony_64kbps.opus"; 

% Audio Type Verification (Stereo Or Mono) 
[x, fs_x] = audioread(file_opus); 
[y, fs_y] = audioread(file_wav);

%% Verificação das freq de amostragem
if fs_x ~= fs_y
    error('Frequências de amostragem diferentes: %d Hz vs %d Hz', fs_x, fs_y);
end

% Passar de stereo para mono
if size(x,2) == 2, x = mean(x,2); end
if size(y,2) == 2, y = mean(y,2); end

% Ajustar o tempo
min_len = min(length(x), length(y));
x = x(1:min_len, :);
y = y(1:min_len, :);

t = (0:min_len-1) / fs_x;

%% Criar figura única
figure;

% --- (1) Comparação no tempo
subplot(3,1,1);
plot(t, y, 'b'); hold on;
plot(t, x, 'r');
xlabel('Tempo [s]'); ylabel('Amplitude');
legend('Original WAV','Comprimido OPUS');
title('Comparação no Tempo');
xlim([0 425]);

% --- (2) Espectrogramas
subplot(3,2,3); % parte esquerda
spectrogram(y, 1024, 512, 1024, fs_x, 'yaxis');
xlim([0 7]);
title('Original WAV');

subplot(3,2,4); % parte direita
spectrogram(x, 1024, 512, 1024, fs_x, 'yaxis');
xlim([0 7]);
title('Comprimido OPUS');

% --- (3) FFT
subplot(3,1,3);
N = length(y);
Y = abs(fft(y));
X = abs(fft(x));
f = (0:N-1)*(fs_x/N);

% Apenas metade útil
Y = Y(1:N/2);
X = X(1:N/2);
f = f(1:N/2)/1000; % converter para kHz

% Converter para dB
YdB = 20*log10(Y + 1e-12);
XdB = 20*log10(X + 1e-12);

% Normalizar para comparar (pico = 0 dB)
YdB = YdB - max(YdB);
XdB = XdB - max(XdB);

plot(f, YdB, 'b'); hold on;
plot(f, XdB, 'r');
xlabel('Frequência [kHz]');
ylabel('Magnitude relativa [dB]');
legend('Original','Comprimido');
title('Espectro em Frequência');
xlim([0 20]);

