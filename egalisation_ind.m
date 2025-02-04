%% Def variables


%%%%%%%%%%%%%%%%%matrice pour les RI

fs = 10;
C = createMatC(16,16);
% Faire un code pour que chaque enregistrement aille dans la bonne colonne
%ou le faire à la main mais chiant

%%%%%%%% Matrice H pour les 16 vecteurs h des filtres
H= cell(16, 1);
h = zeros (10,1);
h_intermediaire = zeros(10, 1);

%%%%%%%%%%%%%%%%% Distance source-enceintes
ri = 0;
%%%%%%%%%%%%%% Distance source-microphones
rj = 0;


%%%%%%%%%%%%%%%%% Position de la source virtuelle
sourcePosZ = 1;
sourcePosX = 1;
sourcePos = [sourcePosX sourcePosZ];

%%%%%%%%%%%%%%%%% Position des enceintes
% z = 0 pour toutes les enceintes
% vecteur pour position en X de chaque enceinte i
speakerPosZ = zeros(1,16);
speakerPosX = linspace (-2, 2, 16);

%%%%%%%%%%%%%%%%% Position des microphones
microPosZ = -1;
microPosX = linspace(-2,2,16);


%% Entree des enregistrement+correction desx niveaux+lissage fréquentiel+stockage dans C 



%% Création des filtres

%boucle sur nb d'enceintes i = 16
for i = 1:16 

    % calcul de ri 
    ri = sqrt((abs(speakerPosX(i) - sourcePosX))^2 + (abs(speakerPosX(i) - sourcePosZ))^2);

    %calcul des h en fonction de l'enceinte
    %enceinte 3 à 14
    if ((i >= 3) && (i < 15))
        J = 5;
        %boucle sur les J microphones qui se trouvent dans l'angle des 60°
        %de l'enceinte i 
        for j = i - floor(J/2) : 1 : i + floor(J/2)
            rj = sqrt((abs(microPosX(j) - speakerPosX(i)))^2 + (abs(microPosZ - speakerPosZ(i)))^2); 
            h_intermediaire = h_intermediaire + (abs(ri - rj))/(C{i,j});
        end
        %calcul filtre final
        h = J .* h_intermediaire;
    end

% enceinte 1 et 2
    if (i == 1 || i == 2)
        %boucle sur les J microphones j
        for j = 1 : i + 2 
            rj = sqrt((abs(microPosX(j) - speakerPosX(i)))^2 + (abs(microPosZ - speakerPosZ(i)))^2); 
            h_intermediaire = h_intermediaire + abs(ri - rj) / C{i,j};
        end
        %calucl filtre final
        h = (i + 2).* h_intermediaire;
    end

% enceinte 14 et 15
    if (i == 15 || i == 16)
        %boucle sur les J microphones j
        for j = i - 2 : 16
            rj = sqrt((abs(microPosX(j) - speakerPosX(i)))^2 + (abs(microPosZ - speakerPosZ(i)))^2); 
            h_intermediaire = h_intermediaire + abs(ri - rj) / C{i,j};
        end
        h = (abs(i - 16) + 3) .* h_intermediaire;
    end


    %J'ajoute mon nouveau filtre dans la matrice des filtres
    H{i,1} = h_intermediaire;

end

%% Transformation pour passer du domaine fréquentiel au domaine temporel