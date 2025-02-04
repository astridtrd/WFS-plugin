%% Egalisation avec compensation de l'erreur moyenne

% Fonction de qualité individuelle
% On commence par définir les termes

%c matrice 16x16 contenant des vecteurs 
%h égalisation individuelle
%u fonction d'alimentation de l'enceinte i 
%a réponse impulsionnelle idéale

% Appel de la fonction d'alimentation 

% Définition des paramètres
J = 16;  % Nombre de positions de micros
I = 16;  % Nombre d'enceintes
c = 343;  % Vitesse du son (m/s)

% Paramètres spécifiques à la fonction d'alimentation
r0 = 1;  % Distance de référence (m)
delta_r0 = 0.1;  % Variation de distance (m)
phi_0 = pi / 4;  % Angle de référence

% Initialisation de la matrice de qualité q_ind
q_ind = zeros(J, 1);  

% Calcul de la fonction de qualité q_ind 
for j = 1:J
    num = 0;  % Numérateur de l'équation (somme des produits)
    denom = 0; % Dénominateur (normalisation)
    
    for i = 1:I
        % Calcul de la fonction d'alimentation u_psi 
        u_psi = (cos(phi_0) / sqrt(r0)) * exp(-1i * r0) ...
              * sqrt(delta_r0 / (r0 + delta_r0)) * sqrt(1i / (2 * pi));
        
        num = num + c(i, j) * hind(i, j) * u_psi;
        denom = denom + abs(a_phi(j));
    end
    
    % Éviter la division par zéro
    if denom ~= 0
        q_ind(j) = num / denom;
    else
        q_ind(j) = 0;
    end
end

% Calcul des filtres corrigés h_comp 
h_mean = zeros(I, 1);  

for i = 1:I
    num = J * hind(i, :);  % Multiplication par J
    denom = sum(q_ind);  % Somme sur les positions de micro
    
    % Éviter la division par zéro
    if denom ~= 0
        h_mean(i) = num / denom;
    else
        h_mean(i) = 0;
    end
end



