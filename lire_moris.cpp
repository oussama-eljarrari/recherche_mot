#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Construit la table des prefixes (fonction prefixe / lps)
// pour le motif. pi[i] = longueur du plus long prefixe du motif
// qui est aussi suffixe du sous-motif motif[0..i].
// Complexite : O(M) ou M = motif.size().
// Aucun hachage utilise ici.
std::vector<int> calculerPrefixe(const std::string& motif) {
    std::vector<int> pi(motif.size(), 0);

    for (std::size_t i = 1; i < motif.size(); ++i) {
        int j = pi[i - 1];

        while (j > 0 && motif[i] != motif[j]) {
            j = pi[j - 1];
        }

        if (motif[i] == motif[j]) {
            ++j;
        }

        pi[i] = j;
    }

    return pi;
}

// Compare texte et motif avec la logique Knuth-Morris-Pratt.
// Retourne vrai uniquement si texte == motif (mot exact).
// On impose texte.size() == motif.size() pour garder la semantique
// "mot exact" de l'ancien programme a table de hachage.
// Complexite : O(L) ou L = texte.size().
bool kmpEquals(const std::string& texte, const std::string& motif,
               const std::vector<int>& pi) {
    if (motif.empty()) {
        return false;
    }

    if (texte.size() != motif.size()) {
        return false;
    }

    std::size_t j = 0;

    for (std::size_t i = 0; i < texte.size(); ++i) {
        while (j > 0 && texte[i] != motif[j]) {
            j = static_cast<std::size_t>(pi[j - 1]);
        }

        if (texte[i] == motif[j]) {
            ++j;

            if (j == motif.size()) {
                return true;
            }
        } else {
            return false;
        }
    }

    return j == motif.size();
}

int main() {
    std::string nomFichier;
    std::string motRecherche;

    std::cout << "Nom du fichier : ";
    std::cin >> nomFichier;

    std::cout << "Mot a rechercher : ";
    std::cin >> motRecherche;

    const std::chrono::steady_clock::time_point debutProcessus =
        std::chrono::steady_clock::now();

    if (motRecherche.empty()) {
        std::cout << "Mot non trouve." << std::endl;
        return 0;
    }

    std::ifstream fichier(nomFichier);

    if (!fichier) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier \""
                  << nomFichier << "\"." << std::endl;
        return 1;
    }

    // Pre-traitement KMP : calcule une seule fois pour le motif.
    const std::vector<int> prefixe = calculerPrefixe(motRecherche);

    std::string ligne;
    std::size_t numeroLigne = 0;
    std::size_t nombreOccurrences = 0;

    while (std::getline(fichier, ligne)) {
        ++numeroLigne;

        std::istringstream fluxLigne(ligne);
        std::string mot;
        std::size_t numeroMot = 0;

        while (fluxLigne >> mot) {
            ++numeroMot;

            if (kmpEquals(mot, motRecherche, prefixe)) {
                ++nombreOccurrences;
                std::cout << "Trouve : ligne " << numeroLigne
                          << ", mot numero " << numeroMot << std::endl;
            }
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

    std::cout << "Temps total du processus avec KMP : "
              << tempsProcessus << " nanosecondes" << std::endl;

    return 0;
}
