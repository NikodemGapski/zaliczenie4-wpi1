#include "kol.h"
#include <cstdlib>
#include <algorithm>
// ---------- DECLARATIONS ----------

// Disoriented list of interesants iterator
class Iterator {
public:
	// create an iterator pointing at interesant el in the direction of the el.adj[adj_idx]
	Iterator(interesant *el, int adj_idx);
	interesant *operator*() const;
	Iterator &operator++();
	// start pointing in the opposite direction
	void flip();
	bool is_end() const;

	interesant *cur() const;
	interesant *prev() const;
private:
	interesant *_cur, *_prev;
};

// Window (distoriented list of interesants with dummy elements)
class Window {
// ----- NON-STATIC MEMBERS -----
public:
	Window();
	~Window();
	void push_back(interesant *el);
	interesant *pop_front();
	// move all interesants from this window to other
	void move_to(Window &other);
	// flip the order of the list
	void flip();
	bool is_empty() const;
	interesant *first() const;
	interesant *last() const;
	interesant *head, *tail;
// ----- STATIC MEMBERS -----
public:
	inline static std::vector<Window> windows = {};
};

// ---------- DEFINITIONS ----------

// --- Iterator ---

Iterator::Iterator(interesant *el, int adj_idx) : _cur(el), _prev(el->adj[1 - adj_idx]) {}

interesant *Iterator::operator*() const { return _cur; }

Iterator &Iterator::operator++() {
	if(!is_end()) {
		interesant *tmp = _cur;
		_cur = _cur->adj_diff(_prev);
		_prev = tmp;
	}
	return *this;
}

void Iterator::flip() { _prev = _cur->adj_diff(_prev); }

bool Iterator::is_end() const { return _cur->adj[0] == NULL; }

interesant *Iterator::cur() const { return _cur; }

interesant *Iterator::prev() const { return _prev; }

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
	delete head;
	delete tail;
}

void Window::push_back(interesant *el) {
	el->adj[0] = tail;
	el->adj[1] = last();
	interesant::connect(last(), tail, el);
	tail->adj[1] = el;
}

interesant *Window::pop_front() {
	if(is_empty()) return NULL;
	interesant *res = first();

	head->adj[1] = res->adj_diff(head); // head now points to where res was pointing
	interesant::connect(first(), res, head);

	return res;
}

void Window::move_to(Window &other) {
	if(is_empty()) return;

	interesant::connect(first(), head, other.last());
	interesant::connect(other.last(), other.tail, first());
	
	interesant::connect(last(), tail, other.tail);
	interesant::connect(other.tail, other.last(), last());

	head->adj[1] = tail;
	tail->adj[1] = head;
}

void Window::flip() { std::swap(head, tail); }

bool Window::is_empty() const { return first() == tail; }

interesant *Window::first() const {
	return head->adj[1];
}
interesant *Window::last() const {
	return tail->adj[1];
}

// --- interesant ---

interesant::interesant(bool not_dummy) : id(not_dummy ? counter++ : -1) {}

int interesant::get_id() const { return id; }

void interesant::exit_queue() {
	connect(adj[0], this, adj[1]);
	connect(adj[1], this, adj[0]);
}

interesant *interesant::adj_diff(interesant *other) const {
	int idx = (adj[0] == NULL); // adj[idx] != NULL
	idx = (adj[idx] == other) ? 1 - idx : idx;
	return adj[idx];
}

void interesant::connect(interesant *cur, interesant *prev, interesant *value) {
	int idx = (cur->adj[0] == NULL); // adj[idx] != NULL
	idx = (cur->adj[idx] == prev) ? idx : 1 - idx; // now adj[idx] is the desired place to put value
	cur->adj[idx] = value;
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
	if(i1 == i2) {
		i1->exit_queue();
		return {i1};
	}
	
	Iterator a(i1, 0), b(i1, 1);
	while(*a != i2 && *b != i2) {
		++a; ++b;
	}
	if(*b == i2) std::swap(a, b);

	a.flip(); Iterator last = a;
	
	std::vector<interesant*> res;
	while(*a != i1) {
		res.push_back(*a);
		++a;
	}
	res.push_back(*a);

	// connect the edges of the interval
	a.flip(); Iterator first = a;
	interesant::connect(last.prev(), last.cur(), first.prev());
	interesant::connect(first.prev(), first.cur(), last.prev());

	std::reverse(res.begin(), res.end());
	return res;
}

void naczelnik(int k) {
	Window::windows[k].flip();
}

std::vector<interesant*> zamkniecie_urzedu() {
	std::vector<interesant*> res;
	for(auto &window : Window::windows) {
		while(!window.is_empty()) {
			res.push_back(window.pop_front());
		}
	}
	Window::windows.clear();
	return res;
}