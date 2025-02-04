function C = createMatC(dim1, dim2)

C = cell(dim1,dim2);
s = "Speaker_";
m = "_mic";

for i=1:dim1
    speaker = append(s, int2str(i));
    if (i>=3 && i < 15)
        J = 5;
        for j = i - floor(J/2) : i + floor (J/2) 
            micro = append(m, int2str(j));
            nom = append(speaker,micro);
            nomwav = append(nom, '.wav');
            %fprintf ('%s \n', nom);
            nom_corrige = append(nom, '_corrige.wav');
            x = calibrage(nomwav, nom_corrige);
            X = fft(x);
            C{i,j} = X;
            % AJOUTER LA FFT A CHAQUE BOUCLE !!!!!
            %X = fft(x);
            %C{i,j} = x; 
        end
    end
    if (i== dim1 - dim1 +1 || i == dim1 - dim1 + 2)
         for j = 1 : i + 2 
            micro = append(m, int2str(j));
            nom = append(speaker,micro);
            %fprintf ('%s \n', nom);
            nom_corrige = append(nom, '_corrige.wav');
            nomwav = append(nom, '.wav');
            x = calibrage(nomwav, nom_corrige);
            X = fft(x);
            C{i,j} = X;     
        end
    end
    if (i == dim1 - 1 || i == dim1)
        %boucle sur les J microphones j
        for j = i - 2 : 16
            micro = append(m, int2str(j));
            nom = append(speaker,micro);
            %fprintf ('%s \n', nom);
            nomwav = append(nom, '.wav');
            nom_corrige = append(nom, '_corrige.wav');
            x = calibrage (nomwav, nom_corrige);
            X = fft(x);
            C{i,j} = X;  
        end
    end

end