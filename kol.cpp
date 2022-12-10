#include "kol.h"
#include <cstdlib>
#include <algorithm>
// ---------- DECLARATIONS ----------

// Desoriented list of interesants iterator
class Iterator {
public:
	// create an iterator pointing at interesant el in the direction of the el.adj[adj_idx]
	Iterator(interesant *el, int adj_idx);
	interesant *operator*() const;
	Iterator& operator++();
	// start pointing in the opposite direction
	void flip();
	bool is_end() const;

	interesant *get_cur() const;
	interesant *get_prev() const;
private:
	interesant *prev, *cur;
};

// Window (Desoriented list of interesants with dummy elements)
class Window {
// ----- NON-STATIC MEMBERS -----
public:
	Window();
	~Window();
	// add el to the back of the list
	void push_back(interesant *el);
	// remove and retrieve the first element from the list
	interesant *pop_front();
	// move all interesants from this window to other
	void move_to(Window& other);
	// flip the order of the list
	void flip();
	bool is_empty() const;
	interesant *head, *tail;
// ----- STATIC MEMBERS -----
public:
	// connect a.prev with b.prev
	static void connect(Iterator a, Iterator b);
	inline static std::vector<Window> windows = {};
};

// ---------- DEFINITIONS ----------

// --- Iterator ---

Iterator::Iterator(interesant *el, int adj_idx) : cur(el), prev(el->adj[1 - adj_idx]) {}

interesant *Iterator::operator*() const { return cur; }

Iterator& Iterator::operator++() {
	if(!is_end()) {
		interesant *tmp = cur;
		cur = cur->adj_not(prev);
		prev = tmp;
	}
	return *this;
}

void Iterator::flip() { prev = cur->adj_not(prev); }

bool Iterator::is_end() const { return cur->adj[0] == NULL; }

interesant *Iterator::get_cur() const { return cur; }

interesant *Iterator::get_prev() const { return prev; }

// --- Window ---

Window::Window() {
	head = new interesant(false);
	tail = new interesant(false);
	head->adj[0] = NULL;
	tail->adj[0] = NULL;
	head->adj[1] = tail;
	tail->adj[1] = head;
}

Window::~Window() {
	// since the user is responsible for deallocating the interesant objects in the list, we only need to deallocate head and tail
	delete head;
	delete tail;
}

void Window::push_back(interesant *el) {
	el->adj[0] = tail;
	el->adj[1] = tail->adj[1];
	tail->adj[1] = el; // tail now points to el
	el->adj[1]->assign_adj_is(tail, el); // the interesant tail was pointing to now points to el
}

interesant *Window::pop_front() {
	if(is_empty()) return NULL;
	interesant *res = head->adj[1];
	head->adj[1] = res->adj_not(head); // head now points to where res was pointing
	head->adj[1]->assign_adj_is(res, head); // the interesant res was pointing to now points to head
	return res;
}

void Window::move_to(Window& other) {
	if(is_empty()) return;
	// the first element from this list and the last element from the other list point to each other
	interesant::connect(head->adj[1], head, other.tail->adj[1], other.tail);
	// the last element from this list and the tail of the other list point to each other
	interesant::connect(tail->adj[1], tail, other.tail, other.tail->adj[1]);
	// head and tail point to each other
	head->adj[1] = tail;
	tail->adj[1] = head;
}

void Window::flip() { std::swap(head, tail); }

bool Window::is_empty() const { return head->adj[1] == tail; }

void Window::connect(Iterator a, Iterator b) {
	interesant::connect(a.get_prev(), a.get_cur(), b.get_prev(), b.get_cur());
}

// --- interesant ---

interesant::interesant(bool not_dummy) : id(not_dummy ? counter++ : -1) {}

int interesant::get_id() const { return id; }

void interesant::exit_queue() {
	adj[0]->assign_adj_is(this, adj[1]);
	adj[1]->assign_adj_is(this, adj[0]);
}

interesant *interesant::adj_not(interesant *other) const {
	int idx = (adj[0] == NULL); // adj[idx] != NULL
	idx = (adj[idx] == other) ? 1 - idx : idx;
	return adj[idx];
}

void interesant::assign_adj_is(interesant *other, interesant *value) {
	int idx = (adj[0] == NULL); // adj[idx] != NULL
	idx = (adj[idx] == other) ? idx : 1 - idx; // now adj[idx] is the desired place to put value
	adj[idx] = value;
}

void interesant::connect(interesant *a, interesant *prev_adj_a, interesant *b, interesant *prev_adj_b) {
	a->assign_adj_is(prev_adj_a, b);
	b->assign_adj_is(prev_adj_b, a);
}

// ---------- USER FUNCTIONS ----------

void otwarcie_urzedu(int m) {
	Window::windows.resize(m);
}

interesant *nowy_interesant(int k) {
	interesant *el = (interesant*)malloc(sizeof(interesant));
	*el = interesant();
	Window::windows[k].push_back(el);
	return el;
}

int numerek(interesant *i) {
	return i->get_id();
}

interesant *obsluz(int k) {
	return Window::windows[k].pop_front();
}

void zmiana_okienka(interesant *i, int k) {
	i->exit_queue();
	Window::windows[k].push_back(i);
}

void zamkniecie_okienka(int k1, int k2) {
	Window::windows[k1].move_to(Window::windows[k2]);
}

std::vector<interesant*> fast_track(interesant *i1, interesant *i2) {
	// if there is just one element to remove, do it manually
	if(i1 == i2) {
		i1->exit_queue();
		return {i1};
	}
	// otherwise move from i1 in both directions until i2 is found
	Iterator a(i1, 0), b(i1, 1);
	while(*a != i2 && *b != i2) {
		++a; ++b;
	}
	if(*b == i2) std::swap(a, b);
	// now iterator a points to i2 in the opposite direction to i1
	a.flip(); Iterator handle_1 = a;
	// move back to i1 and collect the result
	std::vector<interesant*> res;
	while(*a != i1) {
		res.push_back(*a);
		++a;
	}
	res.push_back(*a);
	a.flip(); Iterator handle_2 = a;
	// connect the edges of the interval
	Window::connect(handle_1, handle_2);
	// the elements are in reverse order
	std::reverse(res.begin(), res.end());
	return res;
}

void naczelnik(int k) {
	Window::windows[k].flip();
}

std::vector<interesant*> zamkniecie_urzedu() {
	std::vector<interesant*> res;
	for(auto& window : Window::windows) {
		while(!window.is_empty()) {
			res.push_back(window.pop_front());
		}
	}
	Window::windows.clear();
	return res;
}