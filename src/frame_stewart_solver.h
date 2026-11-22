#pragma once
#include <deque>
#include <vector>

class FrameStewartSolver {
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
        int boxIndex;
        Destination destination;
    };

    FrameStewartSolver();
    std::deque<Movement> solve(int boxes, int towers);
    static int cost(int boxes, int towers);
private:
    int boxes = 1;
    int towers = 3;
    std::deque<Movement> movements;
    void internalSolve(int boxes, int base, std::vector<int>& towers);
    Destination destinationFromTowerIndex(int tower);
};