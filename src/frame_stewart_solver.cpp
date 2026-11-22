#include "frame_stewart_solver.h"
#include <deque>
#include <vector>

FrameStewartSolver::FrameStewartSolver() {}

int FrameStewartSolver::cost(int boxes, int towers) {
    if (boxes <= 1) return boxes;
    if (towers == 3) return (1 << boxes) - 1;
    int best = -1;
    for (int k = 1; k < boxes; k++) {
        int c = 2 * cost(k, towers) + cost(boxes - k, towers - 1);
        if (best == -1 || c < best) best = c;
    }
    return best;
}

std::deque<FrameStewartSolver::Movement> FrameStewartSolver::solve(int boxes, int towers) {
	if (this->boxes == boxes && this->towers == towers) {
		return this->movements;
	}

    this->boxes = boxes;
    this->towers = towers;

    this->movements.clear();
    std::vector<int> all;
    for (int i = 0; i < this->towers; i++) all.push_back(i);
    this->internalSolve(this->boxes, 0, all);
    return this->movements;
}

void FrameStewartSolver::internalSolve(int boxes, int base, std::vector<int>& towers) {
    if (boxes == 0) return;

    if (boxes == 1) {
        this->movements.push_back({ base, this->destinationFromTowerIndex(towers.back()) });
        return;
    }

    int towerCount = towers.size();

    int k = boxes - 1;
    if (towerCount > 3) {
        int best = -1;
        for (int i = 1; i < boxes; i++) {
            int c = 2 * cost(i, towerCount) + cost(boxes - i, towerCount - 1);
            if (best == -1 || c < best) { best = c; k = i; }
        }
    }

    std::vector<int> step1 = towers;
    std::swap(step1[1], step1.back());
    this->internalSolve(k, base, step1);

    std::vector<int> step2 = towers;
    step2.erase(step2.begin() + 1);
    this->internalSolve(boxes - k, base + k, step2);

    std::vector<int> step3 = towers;
    std::swap(step3[0], step3[1]);
    this->internalSolve(k, base, step3);
}

FrameStewartSolver::Destination FrameStewartSolver::destinationFromTowerIndex(int tower) {
    if (tower == 0) return { DockType::Origin, 0 };
    if (tower == this->towers - 1) return { DockType::Destination, 0 };
    return { DockType::Auxiliary, tower - 1 };
}