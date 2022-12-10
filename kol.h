#ifndef KOL_H
#define KOL_H

#include <vector>

// Wszędzie w zadaniu można założyć, że wskaźniki przekazywane do funkcji są
// wskaźnikami na struktury interesant, które były kiedyś wynikiem funkcji
// "nowy_interesant".

// UWAGA: funkcja "numerek" powinna działać również dla interesantów
// niestojących w żadnej kolejce, a nawet po wywołaniu "zamkniecie_urzedu"

struct interesant {
public:
	// construct an interesant object and assign its id
	// not_dummy - whether the created object is not a dummy one
	interesant(bool not_dummy = true);
	// get interesant's id
	int get_id() const;
	// exit the queue the interesant is currently waiting in
	void exit_queue();

	bool operator==(const interesant& other) const;
private:
	// interesant's id
	int id;
	// pointers to adjecent interesants in the queue
	// both pointers are not NULL (exception: in a dummy interesant object, adj[0] == NULL)
	interesant *adj[2];

	// get the index of the adjecent interesant in the queue which is different than other
	int adj_not(interesant *other) const;
	// get the index of the adjecent interesant in the queue which is the same as other
	// (assertion: adj[0] or adj[1] points to the same interesant as other)
	int adj_is(interesant *other) const;
	// the number of non-dummy interesants
	inline static int counter = 0;

	friend class Iterator;
	friend class Window;
};

/**
 * @brief Inicjuje bibliotekę
 *
 * @param m liczba okienek
 */
void otwarcie_urzedu(int m);

/**
 * @brief Do urzędu przychodzi nowy interesant
 *
 * @param k numer okienka, do którego ustawia się nowy interesant
 * @return interesant* wskaźnik na strukturę nowego interesanta
 */
interesant *nowy_interesant(int k);

/**
 * @brief Zwraca numerek interesanta
 *
 * Interesanci dostają numerki będące kolejnymi liczbami całkowitymi, zaczynając
 * od 0
 *
 * UWAGA: musi działać dla interesantów niestojących w żadnej kolejce i po
 * wywołaniu "zamkniecie_urzedu"
 *
 * @param i wskaźnik na interesanta
 * @return int numerek interesanta
 */
int numerek(interesant *i);

/**
 * @brief Obsługuje jednego interesanta
 *
 * @param k numer okienka przy którym obsługiwany jest interesant
 * @return interesant* obsłużony interesant lub NULL jeśli kolejka była pusta
 */
interesant *obsluz(int k);

/**
 * @brief Interesant i ustawia się w kolejce do okienka k
 *
 * Możesz założyć, że wcześniej stał w kolejce do innego okienka
 *
 * @param i interesant zmieniający okienko
 * @param k numer okienka, do którego przechodzi interesant i
 */
void zmiana_okienka(interesant *i, int k);

/**
 * @brief Okienko k1 się zamyka, a interesanci stojący w kolejce przechodzą do
 * okienka k2, w kolejności w jakiej stali
 *
 * @param k1 Zamykane okienko
 * @param k2 Okienko do którego przechodzą interesanci
 */
void zamkniecie_okienka(int k1, int k2);

/**
 * @brief Interesanci od i1 do i2 przechodzą do okienka specjalnego, gdzie są od
 * razu obsługiwani
 *
 * Zakładamy, że i1 oraz i2 stoją w tej samej kolejce oraz że i1 stoi w kolejce
 * przed i2, chyba że i1 == i2.
 *
 * @param i1 Pierwszy ekspresowo obsłużony interesant
 * @param i2 Ostatni ekspresowo obsłużony interesant
 * @return std::vector<interesant *> Wszyscy ekspresowo obsłużeni interesanci w
 * kolejności w której stali w kolejce, zaczynając od i1 a kończąc na i2
 */
std::vector<interesant *> fast_track(interesant *i1, interesant *i2);

/**
 * @brief Naczelnik odwraca kolejność kolejki
 *
 * @param k numer okienka, którego kolejkę odwraca naczelnik
 */
void naczelnik(int k);

/**
 * @brief Zamyka urząd
 *
 * @return std::vector<interesant *> wszyscy interesanci którzy jeszcze stali w
 * kolejkach, uporządkowani wg numeru okienka i następnie porządku kolejki
 */
std::vector<interesant *> zamkniecie_urzedu();

#endif
