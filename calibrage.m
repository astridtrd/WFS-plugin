function y = calibrage(fichier_entree, fichier_sortie)


    % Charger le fichier à traiter
    [x, fs] = audioread(fichier_entree);
    
    % --- ÉTAPE 1 : ÉGALISATION EN FRÉQUENCE ---

    % Charger le fichier enregistré avec le micro de référence
    [x_ref, fs] = audioread('Speaker_1_mic1.wav');

    % Spectre du micro de référence
    X_ref = fft(x_ref);
    
    % Calcul du spectre du signal à corriger
    X = fft(x);
    f = linspace(0, fs/2, length(X)/2+1); % Axe des fréquences

    % Création du filtre inverse
    filtre_inverse = abs(X_ref(1:length(f))) ./ abs(X(1:length(f))); 

    % Assurer que filtre_inverse est un vecteur colonne avant la multiplication
    filtre_inverse = filtre_inverse(:); % Force un vecteur colonne

    % Application du filtre en fréquence
    X_corrige = X(1:length(f)) .* filtre_inverse;
    
    % Reconstruction du signal temporel
    x_corrige_freq = real(ifft([X_corrige; flipud(conj(X_corrige(2:end-1)))]));
    
    % Normalisation pour éviter saturation
    x_corrige_freq = x_corrige_freq / max(abs(x_corrige_freq));

    % --- ÉTAPE 2 : ÉGALISATION EN NIVEAU ---
    
    % Calcul des niveaux RMS en dBFS
    niveau_ref = -18;  % Niveau du fichier référence, choisit pour que les données aient un bon niveau sans pour autant clipper
    niveau_x = 20*log10(rms(x_corrige_freq)); % Niveau après égalisation fréquentielle

    % Calcul du gain nécessaire
    gain_dB = niveau_ref - niveau_x;
    
    % Application du gain
    gain_lin = 10^(gain_dB / 20);
    x_final = x_corrige_freq * gain_lin;
    
    % Sauvegarde du fichier corrigé
    audiowrite(fichier_sortie, x_final, fs);
    [y, fs] = audioread(fichier_sortie);
    %disp(['Égalisation complète appliquée à : ', fichier_entree]);
    %fprintf('Avant égalisation : %.2f dB\n', niveau_x);
    %fprintf('Après égalisation : %.2f dB (cible : %.2f dB)\n', 20*log10(rms(x_final)), niveau_ref);
end




