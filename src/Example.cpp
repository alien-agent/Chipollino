#include "Example.h"

void Example::determinize() {
	vector<State> states;
	for (int i = 0; i < 6; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states.push_back(s);
	}

	states[0].set_transition(5, 'x');
	states[0].set_transition(5, 'y');
	states[0].set_transition(1, '\0');
	states[1].set_transition(2, '\0');
	states[1].set_transition(3, '\0');
	states[1].set_transition(4, '\0');
	states[3].set_transition(3, 'x');
	states[4].set_transition(4, 'y');
	states[4].set_transition(4, 'y');
	states[5].set_transition(5, 'z');
	states[5].set_transition(1, '\0');

	states[2].is_terminal = true;
	states[3].is_terminal = true;
	states[4].is_terminal = true;

	FiniteAutomaton nfa(0, {'x', 'y', 'z'}, states, false);
	cout << nfa.determinize().to_txt();
}

void Example::remove_eps() {
	vector<State> states;
	for (int i = 0; i < 3; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states.push_back(s);
	}

	states[0].set_transition(0, '0');
	states[0].set_transition(1, '\0');
	states[1].set_transition(1, '1');
	states[1].set_transition(2, '\0');
	states[2].set_transition(2, '0');
	states[2].set_transition(2, '1');

	states[2].is_terminal = true;

	FiniteAutomaton nfa(0, {'0', '1'}, states, false);
	cout << nfa.remove_eps().to_txt();
}

void Example::minimize() {
	vector<State> states;
	for (int i = 0; i < 8; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states.push_back(s);
	}

	states[0].set_transition(7, '0');
	states[0].set_transition(1, '1');
	states[1].set_transition(0, '1');
	states[1].set_transition(7, '0');
	states[2].set_transition(4, '0');
	states[2].set_transition(5, '1');
	states[3].set_transition(4, '0');
	states[3].set_transition(5, '1');
	states[4].set_transition(5, '0');
	states[4].set_transition(6, '1');
	states[5].set_transition(5, '0');
	states[5].set_transition(5, '1');
	states[6].set_transition(6, '0');
	states[6].set_transition(5, '1');
	states[7].set_transition(2, '0');
	states[7].set_transition(2, '1');

	states[5].is_terminal = true;
	states[6].is_terminal = true;
	FiniteAutomaton nfa(0, {'0', '1'}, states, false);
	cout << nfa.minimize().to_txt();
}

void Example::intersection() {
	vector<State> states1;
	for (int i = 0; i < 3; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states1.push_back(s);
	}
	vector<State> states2;
	for (int i = 0; i < 3; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states2.push_back(s);
	}

	states1[0].set_transition(0, 'b');
	states1[0].set_transition(1, 'a');
	states1[1].set_transition(1, 'b');
	states1[1].set_transition(2, 'a');
	states1[2].set_transition(2, 'a');
	states1[2].set_transition(2, 'b');

	states1[1].is_terminal = true;

	states2[0].set_transition(0, 'a');
	states2[0].set_transition(1, 'b');
	states2[1].set_transition(1, 'a');
	states2[1].set_transition(2, 'b');
	states2[2].set_transition(2, 'a');
	states2[2].set_transition(2, 'b');

	states2[1].is_terminal = true;

	FiniteAutomaton dfa1 = FiniteAutomaton(0, {'a', 'b'}, states1, false);
	FiniteAutomaton dfa2 = FiniteAutomaton(0, {'a', 'b'}, states2, false);

	cout << FiniteAutomaton::intersection(dfa1, dfa2).to_txt();
}

void Example::regex_parsing() {
	string reg = "((a|b)*c)";
	Regex r;
	if (r.from_string(reg)) {
		cout << "ERROR\n";
		return;
	}
	r.pre_order_travers();
	r.clear();
}

void Example::fa_bisimilar_check() {
	vector<State> states1;
	for (int i = 0; i < 3; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states1.push_back(s);
	}
	states1[0].set_transition(1, 'a');
	states1[0].set_transition(1, '\0');
	states1[0].set_transition(2, 'b');
	states1[1].set_transition(2, 'a');
	states1[1].set_transition(1, 'b');
	states1[2].set_transition(1, 'a');
	states1[2].set_transition(1, '\0');
	states1[2].set_transition(0, 'b');
	states1[0].is_terminal = true;
	states1[2].is_terminal = true;
	FiniteAutomaton fa1(1, {'a', 'b'}, states1, false);

	vector<State> states2;
	for (int i = 0; i < 2; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states2.push_back(s);
	}
	states2[0].set_transition(1, 'a');
	states2[0].set_transition(1, '\0');
	states2[0].set_transition(0, 'b');
	states2[1].set_transition(0, 'a');
	states2[1].set_transition(1, 'b');
	states2[0].is_terminal = true;
	FiniteAutomaton fa2(1, {'a', 'b'}, states2, false);

	cout << FiniteAutomaton::bisimilar(fa1, fa2);
	//правильный ответ true
}

void Example::fa_equal_check() {
	vector<State> states1;
	for (int i = 0; i < 4; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states1.push_back(s);
	}
	states1[0].set_transition(1, 'a');
	states1[0].set_transition(1, 'a');
	states1[0].set_transition(2, 'a');
	states1[1].set_transition(3, 'b');
	states1[2].set_transition(3, 'c');
	FiniteAutomaton fa1(0, {'a', 'b', 'c'}, states1, false);

	vector<State> states2;
	for (int i = 0; i < 4; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states2.push_back(s);
	}
	states2[0].set_transition(1, 'a');
	states2[0].set_transition(1, 'a');
	states2[0].set_transition(2, 'a');
	states2[1].set_transition(3, 'c');
	states2[2].set_transition(3, 'b');
	FiniteAutomaton fa2(0, {'a', 'b', 'c'}, states2, false);

	vector<State> states3;
	for (int i = 0; i < 4; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states3.push_back(s);
	}
	states3[3].set_transition(2, 'a');
	states3[3].set_transition(2, 'a');
	states3[3].set_transition(1, 'a');
	states3[2].set_transition(0, 'b');
	states3[1].set_transition(0, 'c');
	FiniteAutomaton fa3(3, {'a', 'b', 'c'}, states3, false);

	cout << FiniteAutomaton::equal(fa1, fa1) << endl
		 << FiniteAutomaton::equal(fa1, fa2) << endl
		 << FiniteAutomaton::equal(fa1, fa3);
	//правильный ответ 1 0 1
}

void Example::fa_merge_bisimilar() {
	vector<State> states1;
	for (int i = 0; i < 3; i++) {
		State s = {i, {i}, to_string(i), false, map<char, vector<int>>()};
		states1.push_back(s);
	}
	states1[0].set_transition(1, 'a');
	states1[0].set_transition(1, '\0');
	states1[0].set_transition(2, 'b');
	states1[1].set_transition(2, 'a');
	states1[1].set_transition(1, 'b');
	states1[2].set_transition(1, 'a');
	states1[2].set_transition(1, '\0');
	states1[2].set_transition(0, 'b');
	states1[0].is_terminal = true;
	states1[2].is_terminal = true;
	FiniteAutomaton fa1(1, {'a', 'b'}, states1, false);

	cout << fa1.to_txt();

	FiniteAutomaton fa2 = fa1.merge_bisimilar();

	cout << fa2.to_txt();
}