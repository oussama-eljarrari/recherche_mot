#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::string nomFichier;
    std::string motRecherche;

    std::cout << "Nom du fichier : ";
    std::cin >> nomFichier;

    std::cout << "Mot a rechercher : ";
    std::cin >> motRecherche;

    const std::chrono::steady_clock::time_point debutProcessus =
        std::chrono::steady_clock::now();
    std::ifstream fichier(nomFichier);

    if (!fichier) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier \""
                  << nomFichier << "\"." << std::endl;
        return 1;
    }

    std::string ligne;
    std::size_t numeroLigne = 0;
    std::size_t nombreOccurrences = 0;
    while (std::getline(fichier, ligne)) {
        ++numeroLigne;

        std::istringstream fluxLigne(ligne);
        std::vector<std::string> motsDeLaLigne;
        std::string mot;

        while (fluxLigne >> mot) {
            motsDeLaLigne.push_back(mot);
        }

        std::vector<std::string>::const_iterator debutRecherche =
            motsDeLaLigne.cbegin();

        while (debutRecherche != motsDeLaLigne.cend()) {
            const std::vector<std::string>::const_iterator resultat =
                std::find(debutRecherche, motsDeLaLigne.cend(),
                          motRecherche);

            if (resultat == motsDeLaLigne.cend()) {
                break;
            }

            const std::size_t numeroMot =
                static_cast<std::size_t>(
                    resultat - motsDeLaLigne.begin()) + 1;

            ++nombreOccurrences;
            std::cout << "Trouve : ligne " << numeroLigne
                      << ", mot numero " << numeroMot << std::endl;

            debutRecherche = resultat + 1;
        }
    }

    if (nombreOccurrences == 0) {
        std::cout << "Mot non trouve." << std::endl;
    } else {
        std::cout << "Nombre total d'occurrences : "
                  << nombreOccurrences << std::endl;
    }

    const std::chrono::steady_clock::time_point finProcessus =
        std::chrono::steady_clock::now();
    const long long tempsProcessus =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            finProcessus - debutProcessus).count();

    std::cout << "Temps total du processus avec find : "
              << tempsProcessus << " nanosecondes" << std::endl;

    return 0;
}
