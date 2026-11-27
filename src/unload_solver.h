#pragma once
#include <deque>

enum class BoxType {
    Big,
    SmallGroup
};

class UnloadSolver {
public:
    enum class DockType {
        Origin,
        Destination,
        Auxiliary
    };

    struct Destination {
        DockType type;
        int auxiliarIndex;
    };

    struct Movement {
        Destination source;
        Destination destination;
    };

    UnloadSolver();
    std::deque<Movement> solve(int bigBoxes, int smallGroups, int auxiliarDocks);
    static int cost(int bigBoxes, int smallGroups);
private:
    std::deque<Movement> movements;
    void internalSolve(int bigBoxes, int smallGroups, Destination from, Destination to, Destination via);
    void moveBoxes(int count, Destination from, Destination to);
};
