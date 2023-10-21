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

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
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
