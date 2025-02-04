%% Egalisation en fréquence et en niveau de tous les micros 

micros = {'mic_1.wav', 'mic_2.wav', 'mic_3.wav', 'mic_4.wav', 'mic_5.wav', 'mic_6.wav', 'mic_7.wav', 'mic_8.wav', 'mic_9.wav', 'mic_10.wav', 'mic_11.wav', 'mic_12.wav', 'mic_13.wav', 'mic_14.wav', 'mic_15.wav', "mic_16.wav"};

for i = 1:length(micros)
    fichier_sortie = ['corrected_' char(micros{i})];
    egalisation_complete(micros{i}, 'mic_ref.wav', fichier_sortie);
end