#include "unload_solver.h"
#include <deque>

UnloadSolver::UnloadSolver() {}

int UnloadSolver::cost(int bigBoxes, int smallGroups) {
    if (bigBoxes == 0) return smallGroups == 0 ? 0 : 1 + cost(0, smallGroups - 1);
    if (bigBoxes == 1) return 1 + 2 * cost(0, smallGroups);
    return 1 + cost(bigBoxes - 1, smallGroups);
}

std::deque<UnloadSolver::Movement> UnloadSolver::solve(int bigBoxes, int smallGroups, int auxiliarDocks) {
    Destination origin = { DockType::Origin, 0 };
    Destination destination = { DockType::Destination, 0 };
    Destination auxiliar = { DockType::Auxiliary, 0 };

    this->movements.clear();

    if (bigBoxes > 0 && smallGroups > 0 && auxiliarDocks <= 0) return this->movements;

    this->internalSolve(bigBoxes, smallGroups, origin, destination, auxiliar);
    return this->movements;
}

void UnloadSolver::internalSolve(int bigBoxes, int smallGroups, Destination from, Destination to, Destination via) {
    if (bigBoxes == 0) {
        this->moveBoxes(smallGroups, from, to);
        return;
    }

    this->internalSolve(0, smallGroups, from, via, to);
    this->moveBoxes(bigBoxes, from, to);
    this->internalSolve(0, smallGroups, via, to, from);
}

void UnloadSolver::moveBoxes(int count, Destination from, Destination to) {
    if (count == 0) return;
    this->movements.push_back({ from, to });
    this->moveBoxes(count - 1, from, to);
}
