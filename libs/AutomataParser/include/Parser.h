#pragma once

#include <Objects/FiniteAutomaton.h>
#include <Objects/MemoryFiniteAutomaton.h>
#include <vector>
#include <map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <Lexer.h>
#include <lexy/callback/string.hpp>
#include <lexy/lexeme.hpp>
#include <lexy/callback/string.hpp>
#define child lexy::_pt_node<lexy::_bra, void>

class Parser {
  private:
    static std::vector<child> find_children(utf8_tree& tree, std::set<std::string> names, std::set<std::string> exclude = {}) {
        std::vector<child> result;
        int skip = 0;
        for (auto [event, node] : tree.traverse())
        {
            auto depth = 0;
            switch (event)
            {
            case lexy::traverse_event::enter:
                if (!skip && names.count(node.kind().name())) {
                    result.push_back(node);
                }
                if (exclude.count(node.kind().name())) {
                    skip++;             
                }
                depth++;
                break;
            case lexy::traverse_event::exit:
                if (exclude.count(node.kind().name())) {
                    skip--;                       
                }
                depth--;
                break;

            case lexy::traverse_event::leaf:
                if (!skip && names.count(node.kind().name())) {
                    result.push_back(node);
                }
                break;
            }
        }
        return result;
    }

    // std::vector<FAState> parse_state_description(utf8_tree& tree) {
    //     std::vector<std::string> names;
    //     auto nodes = find_children(tree, {"node_id"}, {"state_label"});
    //     for (int i = 0; i < nodes.size(); i++) {
    //         names.push_back(lexy::as_string<std::string, lexy::utf8_encoding>(nodes[i].children().begin()->lexeme()));
    //     }
    // };

    static void parse_states(utf8_tree& tree, std::set<std::string>& names, std::map<std::string, std::string>& labels) {
        auto nodes = find_children(tree, {"node_id"}, {"state_label"});
        for (int i = 0; i < nodes.size(); i++) {
            auto lexeme = nodes[i].children().begin()->token().lexeme();
            auto name = lexy::as_string<std::string, lexy::ascii_encoding>(lexeme);

            names.insert(name);
            labels[name] = name;
        }
    }

    static void parse_descriptions(utf8_tree& tree, std::map<std::string, std::string>& labels, std::map<std::string, bool>& is_terminal, std::string& initial) {
        auto descriptions = find_children(tree, {"state_description"});
        for (int i = 0; i < descriptions.size(); i++) {
            std::string name = "";
            std::cout << "ds: " << descriptions[i].children().size() << "\n";
            for (auto desc : descriptions[i].children()) {
                if (std::string(desc.kind().name()) == "node_id")
                    name = lexy::as_string<std::string, lexy::ascii_encoding>(desc.children().begin()->lexeme());
                if (std::string(desc.kind().name()) == "state_label")
                    for (auto alias_child : desc.children())
                        if (std::string(alias_child.kind().name()) == "node_id")
                            labels[name] = lexy::as_string<std::string, lexy::ascii_encoding>(alias_child.children().begin()->token().lexeme());
                if (std::string(desc.kind().name()) == "terminal_mark")
                    is_terminal[name] = true;
                if (std::string(desc.kind().name()) == "initial_mark")
                    if (initial == "")
                        initial = name;
                    else
                        throw std::runtime_error("AutomataParser::Parser::parse_descriptions ERROR(second initial state found)");
            }
        }
    }

    static void parse_transitions(utf8_tree& tree, std::vector<std::string>& beg, std::vector<std::string>& end, std::vector<std::string>& symb) {
        auto transitions = find_children(tree, {"stmt"});
        for (int i = 0; i < transitions.size(); i++) {
            std::cout << transitions[i].kind().name();
            auto it = transitions[i].children().begin();
            beg.push_back(lexy::as_string<std::string, lexy::ascii_encoding>(it->children().begin()->lexeme()));
            it++;
            end.push_back(lexy::as_string<std::string, lexy::ascii_encoding>(it->children().begin()->lexeme()));
            it++;
            symb.push_back(lexy::as_string<std::string, lexy::ascii_encoding>(it->children().begin()->lexeme()));
        }
    }

    struct MFATransition_info{
        std::string beg;
        std::string end;
        Symbol symb;

        std::unordered_set<int> open;
        std::unordered_set<int> close;
    };

    static std::vector<MFATransition_info> parse_MFA_transitions(utf8_tree& tree) {
        auto edges = find_children(tree, {"MFA_edge"});
        std::vector<MFATransition_info> mfat_info;

        std::cout << edges.size() << "edegewqw\n";
        for (int i = 0; i < edges.size(); i++) {
            auto edge_child = edges[i].children().begin();
            std::cout << edge_child->kind().name();
            auto it = edge_child->children().begin();
            std::string beg = lexy::as_string<std::string, lexy::ascii_encoding>(it->children().begin()->lexeme());
            it++;
            std::string end = lexy::as_string<std::string, lexy::ascii_encoding>(it->children().begin()->lexeme());
            it++;
            Symbol symb = lexy::as_string<std::string, lexy::ascii_encoding>(it->children().begin()->lexeme());
            
            if (symb == "&") {
                for (auto symb_child : it->children()) {
                    if (std::string(symb_child.kind().name()) == "cell_id") {
                        symb = Symbol::Ref(std::stoi(lexy::as_string<std::string, lexy::ascii_encoding>(symb_child.children().begin()->lexeme())));
                    }
                }
            }
            if (symb == "eps")
                symb = Symbol::Epsilon;

            std::unordered_set<int> open;
            std::unordered_set<int> close;
            edge_child++;
            std::cout << edge_child->kind().name();
            for (auto memory_cell : edge_child->children()) {
                if (std::string(memory_cell.kind().name()) == "memory_cell") {
                    auto cell = memory_cell.children().begin();
                    int cell_id = std::stoi(lexy::as_string<std::string, lexy::ascii_encoding>(cell->children().begin()->lexeme()));
                    cell++;
                    if (lexy::as_string<std::string, lexy::ascii_encoding>(cell->children().begin()->lexeme()) == "c")
                        close.insert(cell_id);
                    if (lexy::as_string<std::string, lexy::ascii_encoding>(cell->children().begin()->lexeme()) == "o")
                        open.insert(cell_id);
                }
            }

            mfat_info.push_back({beg, end, symb, open, close});
        }

        return mfat_info;
    }


  public:
    static void parse_MFA(utf8_tree& tree) {
        std::set<std::string> names;
        std::map<std::string, std::string> labels;
        std::map<std::string, bool> is_terminal;
        std::string initial = "";

        parse_states(tree, names, labels);
        parse_descriptions(tree, labels, is_terminal, initial);

        std::vector<MFAState> states;
        std::map<std::string, int> states_id;

        int k = 0;
        int initial_state = 0;
        for (auto name : names) {
            if (name == initial)
                initial_state = k;
            std:: cout << name << " " << k << "\n";
            states_id[name] = k;
            states.push_back(MFAState(k++, labels[name], is_terminal[name]));
            std::cout << labels[name] << " " << k << "\n";
        }

        std::set<Symbol> alphabet;

        std::cout << "transitions\n";

        auto mfat_info = parse_MFA_transitions(tree);

        for (int i = 0; i < mfat_info.size(); i++) {
            if (!mfat_info[i].symb.is_epsilon() && !mfat_info[i].symb.is_ref()) {
                alphabet.insert(mfat_info[i].symb);
            }

            auto x = MFATransition(states_id[mfat_info[i].end], mfat_info[i].open, mfat_info[i].close);
            states[states_id[mfat_info[i].beg]].set_transition(x, mfat_info[i].symb);
        }

        auto mfa = MemoryFiniteAutomaton(initial_state, states, alphabet);

        std::cout << "\n" << mfa.to_txt();
    }

    static FiniteAutomaton parse_FA(utf8_tree& tree) {
        std::set<std::string> names;
        std::map<std::string, std::string> labels;
        std::map<std::string, bool> is_terminal;
        std::string initial = "";

        parse_states(tree, names, labels);
        parse_descriptions(tree, labels, is_terminal, initial);

        std::vector<FAState> states;
        std::map<std::string, int> states_id;

        int k = 0;
        int initial_state = 0;
        for (auto name : names) {
            if (name == initial)
                initial_state = k;
            std:: cout << name << " " << k << "\n";
            states_id[name] = k;
            states.push_back(FAState(k++, labels[name], is_terminal[name]));
        }

        std::set<Symbol> alphabet;
        std::vector<std::string> beg, end, symb;
        for (int i = 0; i < beg.size(); i++) {
            if (symb[i] == "eps") {
                states[states_id[beg[i]]].set_transition(states_id[end[i]], Symbol::Epsilon);
            }
            else {
                if (symb[i][0] == '&') {
                    throw std::runtime_error("AutomataParser::Parser::parse_FA ERROR(MFA transition found)");
                }
                else {
                    states[states_id[beg[i]]].set_transition(states_id[end[i]], symb[i]);
                    alphabet.insert(Symbol(symb[i]));
                }
            }
        }

        auto fa = FiniteAutomaton(initial_state, states, alphabet);

        return fa;
    }
};