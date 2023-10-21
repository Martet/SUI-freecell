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

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	auto compare_fn = [](const std::pair<double, std::pair<SearchState, std::vector<SearchAction>>> &a, const std::pair<double, std::pair<SearchState, std::vector<SearchAction>>> &b) {
		return a.first > b.first;
	};
	std::priority_queue<std::pair<double, std::pair<SearchState, std::vector<SearchAction>>>, std::vector<std::pair<double, std::pair<SearchState, std::vector<SearchAction>>>>, decltype(compare_fn)> open(compare_fn);
	std::set<SearchState> closed;
	std::map<SearchState, int> g;
	open.push(std::pair(0, std::pair(init_state, std::vector<SearchAction>())));
	while (!open.empty()) {
		auto top_f = open.top().first;
		auto state = open.top().second;
		open.pop();
		if (state.first.isFinal())
			return state.second;
		auto insert = closed.insert(state.first);
		if (!insert.second)
			continue;

		auto actions = state.first.actions();
		for (auto action: actions) {
			auto new_state = action.execute(state.first);
			int new_g = g[state.first] + 1;
			double f = new_g + compute_heuristic(new_state, *heuristic_);
			if (g.count(new_state))
				if (new_g < g[new_state] || (new_g == g[new_state] && f < top_f))
					continue;
			g[new_state] = new_g;
			auto new_actions(state.second);
			new_actions.push_back(action);
			open.push(std::pair(f, std::pair(new_state, new_actions)));
		}

		if (getCurrentRSS() > mem_limit_ - 50 * 1000 * 1000)
			return {};
	}
	return {};
}
