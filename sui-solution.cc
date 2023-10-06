#include <algorithm>
#include <queue>
#include <set>

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
		if (actions.size() == 0)
			continue;
		for (auto action: actions) {
			auto new_state = action.execute(state.first);
			auto new_actions(state.second);
			new_actions.push_back(action);
			if (new_state.isFinal())
				return new_actions;
			open.push(std::pair(new_state, new_actions));
		}
	}
	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	std::vector<std::pair<SearchState, std::vector<SearchAction>>> open;
	std::set<SearchState> closed;
	open.push_back(std::pair(init_state, std::vector<SearchAction>()));

	while (!open.empty()) {
		auto state = open.back();
		open.pop_back();
		auto insert = closed.insert(state.first);
		if (!insert.second)
			continue;

		auto actions = state.first.actions();
		if (actions.size() == 0)
			continue;
		if (state.second.size() >= (std::size_t) depth_limit_)
			continue;
		for (auto action: actions) {
			auto new_state = action.execute(state.first);
			auto new_actions(state.second);
			new_actions.push_back(action);
			if (new_state.isFinal())
				return new_actions;
			open.push_back(std::pair(new_state, new_actions));
		}
	}
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	return {};
}
