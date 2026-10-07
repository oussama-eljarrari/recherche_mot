#include <fstream>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Position {
    std::size_t ligne;
    std::size_t numeroMot;
};

struct Entree {
    std::string mot;
    std::vector<Position> positions;
    bool utilisee = false;
};

class TableHachage {
private:
    std::vector<Entree> table;
    std::size_t nombreElements = 0;

    std::size_t hachage(const std::string& mot) const {
        std::uint64_t hash = 1469598103934665603ULL;

        for (unsigned char caractere : mot) {
            hash ^= caractere;
            hash *= 1099511628211ULL;
        }

        return static_cast<std::size_t>(hash % table.size());
    }

    void agrandir() {
        const std::vector<Entree> ancienneTable = table;

        table.clear();
        table.resize(ancienneTable.size() * 2);
        nombreElements = 0;

        for (const Entree& entree : ancienneTable) {
            if (!entree.utilisee) {
                continue;
            }

            for (const Position& position : entree.positions) {
                ajouterMot(entree.mot, position);
            }
        }
    }

public:
    explicit TableHachage(std::size_t tailleInitiale = 1000003)
        : table(tailleInitiale) {
    }

    void ajouterMot(const std::string& mot, Position position) {
        if (nombreElements * 2 >= table.size()) {
            agrandir();
        }

        std::size_t index = hachage(mot);

        while (table[index].utilisee) {
            if (table[index].mot == mot) {
                table[index].positions.push_back(position);
                return;
            }

            index = (index + 1) % table.size();
        }

        table[index].mot = mot;
        table[index].positions.push_back(position);
        table[index].utilisee = true;
        ++nombreElements;
    }

    const std::vector<Position>* rechercher(
        const std::string& mot) const {
        std::size_t index = hachage(mot);

        while (table[index].utilisee) {
            if (table[index].mot == mot) {
                return &table[index].positions;
            }

            index = (index + 1) % table.size();
        }

        return nullptr;
    }
};

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

    TableHachage index;
    std::string ligne;
    std::size_t numeroLigne = 0;

    while (std::getline(fichier, ligne)) {
        ++numeroLigne;

        std::istringstream fluxLigne(ligne);
        std::string mot;
        std::size_t numeroMot = 0;

        while (fluxLigne >> mot) {
            ++numeroMot;
            index.ajouterMot(mot, {numeroLigne, numeroMot});
        }
    }

    const std::vector<Position>* resultats =
        index.rechercher(motRecherche);

    if (resultats == nullptr) {
        std::cout << "Mot non trouve." << std::endl;
    } else {
        std::cout << "Mot trouve " << resultats->size()
                  << " fois :" << std::endl;

        for (const Position& position : *resultats) {
            std::cout << "Ligne " << position.ligne
                      << ", mot numero " << position.numeroMot
                      << std::endl;
        }
    }

    const std::chrono::steady_clock::time_point finProcessus =
        std::chrono::steady_clock::now();
    const long long tempsProcessus =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            finProcessus - debutProcessus).count();

    std::cout << "Temps total du processus avec table de hachage : "
              << tempsProcessus << " nanosecondes" << std::endl;

    return 0;
}
