#ifndef PROJECT_MAIN_H
#define PROJECT_MAIN_H

struct move {
    int startR, startC, endR, endC, parent;
    bool jump;
    //Actual start locations (in case of multi jumps)
    int actStartR, actStartC;

    move(int sR, int sC, int eR, int eC, int p, bool j) {
        startR = sR;
        startC = sC;
        endR = eR;
        endC = eC;
        parent = p;
        jump = j;
        actStartR = sR;
        actStartC = sC;
    }
};

class main {
public:
    static void mouseClick(int row, int col);
    static std::vector<move> getMoves();

private:
    static void endTurn();
};

#endif //PROJECT_MAIN_H
