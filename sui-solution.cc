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
		if (state.first.isFinal())
			return state.second;
		auto insert = closed.insert(state.first);
		if (!insert.second)
			continue;

		auto actions = state.first.actions();
		if (actions.size() == 0)
			continue;
		for (auto action: actions) {
			auto new_actions(state.second);
			new_actions.push_back(action);
			open.push(std::pair(action.execute(state.first), new_actions));
		}
	}
	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	return {};
}
