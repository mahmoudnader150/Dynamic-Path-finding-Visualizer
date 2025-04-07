#pragma once

namespace Constants {
    const int ROWS = 16;
    const int COLS = 16;
    const int TILE_SIZE = 32;
    const int TOP_BAR_HEIGHT = 60;
    const int BOTTOM_BAR_HEIGHT = 40;
    const int WINDOW_WIDTH = COLS * TILE_SIZE;
    const int WINDOW_HEIGHT = ROWS * TILE_SIZE + TOP_BAR_HEIGHT + BOTTOM_BAR_HEIGHT;

    enum CellType {
        EMPTY = 0,
        WALL = 1,
        START = 2,
        GOAL = 3,
        PATH = 4
    };

    enum AlgorithmType {
        DFS,
        BFS,
        DIJKSTRA
    };
}