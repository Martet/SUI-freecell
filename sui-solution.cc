#include <algorithm>
#include <queue>
#include <set>

#include "memusage.h"
#include "search-strategies.h"

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
	std::queue<std::pair<SearchState, std::vector<SearchAction>>> open;
	std::set<SearchState> closed;
	open.push(std::pair(init_state, std::vector<SearchAction>()));

	while (!open.empty()) {
		auto state = open.front();
		open.pop();
		auto insert = closed.insert(state.first);
		if (!insert.second)
			continue;

		auto actions = state.first.actions();
		for (auto action: actions) {
			auto new_state = action.execute(state.first);
			auto new_actions(state.second);
			new_actions.push_back(action);
			if (new_state.isFinal())
				return new_actions;
			open.push(std::pair(new_state, new_actions));
		}

		if (getCurrentRSS() > mem_limit_ - 50 * 1000 * 1000)
			return {};
	}
	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	std::vector<std::pair<SearchState, std::vector<SearchAction>>> open;
	open.push_back(std::pair(init_state, std::vector<SearchAction>()));

	while (!open.empty()) {
		auto state = open.back();
		open.pop_back();

		if (state.second.size() >= depth_limit_)
			continue;
		auto actions = state.first.actions();
		for (auto action: actions) {
			auto new_state = action.execute(state.first);
			auto new_actions(state.second);
			new_actions.push_back(action);
			if (new_state.isFinal())
				return new_actions;
			open.push_back(std::pair(new_state, new_actions));
		}

		if (getCurrentRSS() > mem_limit_ - 50 * 1000 * 1000)
			return {};
	}
	return {};
}

int color_to_int(Color color) {
	switch (color) {
		case Color::Heart:
			return 0;
		case Color::Diamond:
			return 1;
		case Color::Club:
			return 2;
		case Color::Spade:
			return 3;
	}
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
	int cards_out_of_home = king_value * colors_list.size();
    for (const auto &home : state.homes) {
        auto opt_top = home.topCard();
        if (opt_top.has_value())
            cards_out_of_home -= opt_top->value;
    }

	int well_placed = 0;
	for (auto &stack: state.stacks) {
		well_placed += stack.nbCards();
		auto last_color = Color::Spade;
		auto last_value = 0;
		for (Card card: stack.storage()) {
			if (card.value < last_value && color_to_int(card.color) / 2 == color_to_int(last_color) / 2)
				well_placed--;
			last_color = card.color;
			last_value = card.value;
		}
	}

	double stack_avg = 0, home_avg = 0;
	for (auto &stack: state.stacks)
		if (stack.topCard().has_value())
			stack_avg += stack.topCard().value().value;
	stack_avg /= 8;
	for (auto &home: state.homes)
		if (home.topCard().has_value())
			home_avg += home.topCard().value().value;
	home_avg /= 4;

	int free_cells = 0;
	for (auto &stack: state.free_cells)
		if (stack.topCard().has_value())
			free_cells++;

    return cards_out_of_home + well_placed + stack_avg - home_avg - free_cells;
}

struct SearchNode {
	int g;
	std::shared_ptr<SearchNode> parent;
	std::shared_ptr<SearchAction> action;
};

std::vector<SearchAction> reconstruct_path(std::shared_ptr<SearchNode> node) {
	std::vector<SearchAction> path;
	while (node->action) {
		path.push_back(*(node->action));
		node = node->parent;
	}
	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	auto compare_fn = [](const std::pair<double, SearchState> &a, const std::pair<double, SearchState> &b) {
		return a.first > b.first;
	};
	std::priority_queue<std::pair<double, SearchState>, std::deque<std::pair<double, SearchState>>, decltype(compare_fn)> open(compare_fn);
	std::set<SearchState> closed;
	std::map<SearchState, SearchNode> nodes;
	nodes[init_state] = {0, nullptr, nullptr};
	open.push(std::pair(0, init_state));
	while (!open.empty()) {
		auto top_f = open.top().first;
		auto state = open.top().second;
		open.pop();
		auto insert = closed.insert(state);
		if (!insert.second)
			continue;

		int new_g = nodes[state].g + 1;
		auto actions = state.actions();
		for (auto action: actions) {
			auto new_state = action.execute(state);
			double f = new_g + compute_heuristic(new_state, *heuristic_);
			struct SearchNode node = {new_g, std::make_shared<SearchNode>(nodes[state]), std::make_shared<SearchAction>(action)};
			if (nodes.count(new_state))
				if (new_g > nodes[new_state].g || (new_g == nodes[new_state].g && f > top_f))
					continue;
			if (new_state.isFinal()){
				std::cout << getCurrentRSS() << std::endl;
				return reconstruct_path(std::make_shared<SearchNode>(node));
			}
			nodes[new_state] = node;
			open.push(std::pair(f, new_state));
		}

		if (getCurrentRSS() > mem_limit_ - 50 * 1000 * 1000)
			return {};
	}
	return {};
}
