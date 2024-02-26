#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <functional>

#define umap unordered_map
#define pos pair<int, int>

using namespace std;

enum COLORS {
    COLORS_WHITE,
    COLORS_BLACK
};

enum TYPES {
    TYPES_KING,
    TYPES_ROOK,
    TYPES_KNIGHT,
    TYPES_BISHEP,
    TYPES_PAWN,
    TYPES_EMPTY
};

umap<TYPES, char> SYMBOLS = {
    {TYPES_KING, 'K'},
    {TYPES_ROOK, 'R'},
    {TYPES_KNIGHT, 'N'},
    {TYPES_BISHEP, 'B'},
    {TYPES_PAWN, 'P'}
};

struct Figure {
    COLORS color;
    TYPES type;
    int cost = 0;
    int x, y;

    Figure(COLORS clr, TYPES tp, int cst, int x1, int y1) :
        color(clr),
        type(tp),
        cost(cst),
        x(x1),
        y(y1) {}

    pos getCoords() const {
        return {y, x};
    }
};

struct Rook : public Figure {
    Rook(const COLORS clr, const int y1, const int x1) : Figure(
        clr,
        TYPES_ROOK,
        5,
        x1,
        y1
    ) {}
};

struct Knight : public Figure {
    public:
    Knight(COLORS clr, int y1, int x1) : Figure(
        clr,
        TYPES_KNIGHT,
        3,
        x1,
        y1
    ) {}
};

struct Bishep : public Figure {
    Bishep(COLORS clr, int y1, int x1) : Figure(
        clr,
        TYPES_BISHEP,
        3,
        x1,
        y1
    ) {}
};

struct Pawn : public Figure {
    Pawn(COLORS clr, int y1, int x1) : Figure(
        clr,
        TYPES_PAWN,
        1,
        x1,
        y1
    ) {}
};

struct King : public Figure {
    King(COLORS clr, int y1, int x1) : Figure(
        clr,
        TYPES_KING,
        100,
        x1,
        y1
    ) {}
};

struct Move {
    pos coordsFrom;
    pos coordsTo;
    Figure* attacker;
    Figure* victim;
    bool check;

    string notation() const {
        string ans = "";

        ans += char(SYMBOLS[attacker->type] + (attacker->color == COLORS_BLACK ? ('a' - 'A') : 0));

        ans += char(coordsFrom.second + 'a');
        ans += char(coordsFrom.first + '1');

        ans += (victim == nullptr ? '-' : 'x');

        ans += char(coordsTo.second + 'a');
        ans += char(coordsTo.first + '1');

        if (check) {
            ans += '+';
        }

        return ans;
    }
};

struct Board {
    int size = 8;
    vector<vector<Figure*>> board;
    umap<COLORS, King*> kings;
    umap<COLORS, vector<Figure*>> figures;
    COLORS colorMove = COLORS_WHITE;
    vector<Move> history;

    Board() {
        board.resize(size);

        for (auto& row : board) {
            row.resize(size, nullptr);
        }

        board[0][0] = new Rook(COLORS_WHITE, 0, 0);
        board[0][size - 1] = new Rook(COLORS_WHITE, 0, size - 1);
        board[0][1] = new Knight(COLORS_WHITE, 0, 1);
        board[0][size - 2] = new Knight(COLORS_WHITE, 0, size - 2);
        board[0][2] = new Bishep(COLORS_WHITE, 0, 2);
        board[0][size - 3] = new Bishep(COLORS_WHITE, 0, size - 3);

        kings[COLORS_WHITE] = new King(COLORS_WHITE, 0, size - 4);
        board[0][size - 4] = kings[COLORS_WHITE];

        for (int i = 0; i < size; i++) {
            if (board[0][i] != nullptr) {
                figures[COLORS_WHITE].push_back(board[0][i]);
            }
        }

        board[size - 1][0] = new Rook(COLORS_BLACK, size - 1, 0);
        board[size - 1][size - 1] = new Rook(COLORS_BLACK, size - 1, size - 1);
        board[size - 1][1] = new Knight(COLORS_BLACK, size - 1, 1);
        board[size - 1][size - 2] = new Knight(COLORS_BLACK, size - 1, size - 2);
        board[size - 1][2] = new Bishep(COLORS_BLACK, size - 1, 2);
        board[size - 1][size - 3] = new Bishep(COLORS_BLACK, size - 1, size - 3);

        kings[COLORS_BLACK] = new King(COLORS_BLACK, size - 1, size - 4);;
        board[size - 1][size - 4] = kings[COLORS_BLACK];

        for (int i = 0; i < size; i++) {
            if (board[size - 1][i] != nullptr) {
                figures[COLORS_BLACK].push_back(board[size - 1][i]);
            }
        }

        for (int i = 0; i < size; i++) {
            board[1][i] = new Pawn(COLORS_WHITE, 1, i);
            board[size - 2][i] = new Pawn(COLORS_BLACK, size - 2, i);

            figures[COLORS_WHITE].push_back(board[1][i]);
            figures[COLORS_BLACK].push_back(board[size - 2][i]);
        }
    }

    ~Board() {
        for (const auto& figure: figures[COLORS_WHITE]) {
            delete figure;
        }
        for (const auto& figure: figures[COLORS_BLACK]) {
            delete figure;
        }
    }

    vector<pos> makeRookMoves(const Figure* figure) const {
        vector<pos> mvs;

        // right
        for (int x = figure->x + 1; x < size; x++) {
            if (board[figure->y][x] != nullptr) {
                if (figure->color != board[figure->y][x]->color) {
                    mvs.push_back({figure->y, x});
                }

                break;
            }

            mvs.push_back({figure->y, x});
        }

        // left
        for (int x = figure->x - 1; x >= 0; x--) {
            if (board[figure->y][x] != nullptr) {
                if (figure->color != board[figure->y][x]->color) {
                    mvs.push_back({figure->y, x});
                }

                break;
            }

            mvs.push_back({figure->y, x});
        }

        // up
        for (int y = figure->y - 1; y >= 0; y--) {
            if (board[y][figure->x] != nullptr) {
                if (figure->color != board[y][figure->x]->color) {
                    mvs.push_back({y, figure->x});
                }

                break;
            }

            mvs.push_back({y, figure->x});
        }

        // down
        for (int y = figure->y + 1; y < size; y++) {
            if (board[y][figure->x] != nullptr) {
                if (figure->color != board[y][figure->x]->color) {
                    mvs.push_back({y, figure->x});
                }

                break;
            }

            mvs.push_back({y, figure->x});
        }

        return mvs;
    }

    vector<pos> makeKnightMoves(const Figure* figure) const {
        vector<pos> mvs;
        pos add = {1,2};
        int x, y;

        for (int a = -1; a < 2; a += 2) {
            for (int b = -1; b < 2; b += 2) {
                x = figure->x + a * add.first;
                y = figure->y + b * add.second;

                if (x >= 0 && x < size && y >= 0 && y < size) {
                    if (board[y][x] == nullptr || board[y][x]->color != figure->color) {
                        mvs.push_back({y, x});
                    }
                }

                x = figure->x + b * add.second;
                y = figure->y + a * add.first;

                if (x >= 0 && x < size && y >= 0 && y < size) {
                    if (board[y][x] == nullptr || board[y][x]->color != figure->color) {
                        mvs.push_back({y, x});
                    }
                }
            }
        }

        return mvs;
    }

    vector<pos> makeBishepMoves(const Figure* figure) const {
        vector<pos> mvs;

        // right down
        for (int x = figure->x + 1, y = figure->y + 1; x < size && y < size; ) {
            if (board[y][x] != nullptr) {
                if (figure->color != board[y][x]->color) {
                    mvs.push_back({y, x});
                }

                break;
            }

            mvs.push_back({y, x});

            x++;
            y++;
        }

        // right up
        for (int x = figure->x + 1, y = figure->y - 1; x < size && y >= 0; ) {
            if (board[y][x] != nullptr) {
                if (figure->color != board[y][x]->color) {
                    mvs.push_back({y, x});
                }

                break;
            }

            mvs.push_back({y, x});

            x++;
            y--;
        }

        // left down
        for (int x = figure->x - 1, y = figure->y + 1; x >= 0 && y < size; ) {
            if (board[y][x] != nullptr) {
                if (figure->color != board[y][x]->color) {
                    mvs.push_back({y, x});
                }

                break;
            }

            mvs.push_back({y, x});

            x--;
            y++;
        }

        // left up
        for (int x = figure->x - 1, y = figure->y - 1; x >= 0 && y >= 0; ) {
            if (board[y][x] != nullptr) {
                if (figure->color != board[y][x]->color) {
                    mvs.push_back({y, x});
                }

                break;
            }

            mvs.push_back({y, x});

            x--;
            y--;
        }

        return mvs;
    }

    vector<pos> makePawnMoves(const Figure* figure) const {
        vector<pos> mvs;
        int x = figure->x;
        int y = figure->y;
        int direction = 1;

        if (figure->color == COLORS_BLACK) {
            direction *= -1;
        }

        if ((y + direction) % size == 0) {
            return {};
        }

        if (x - 1 >= 0 && board[y + direction][x - 1] != nullptr && board[y + direction][x - 1]->color != figure->color) {
            mvs.push_back({y + direction, x - 1});
        }

        if (x + 1 < size && board[y + direction][x + 1] != nullptr && board[y + direction][x + 1]->color != figure->color) {
            mvs.push_back({y + direction, x + 1});
        }

        if (board[y + direction][x] == nullptr) {
            mvs.push_back({y + direction, x});

            if ((y - direction) % size == 0 && board[y + 2 * direction][x] == nullptr) {
                mvs.push_back({y + 2 * direction, x});
            }
        }

        return mvs;
    }

    vector<pos> makeKingMoves(const Figure* figure) const {
        vector<pos> mvs;
        int x = figure->x;
        int y = figure->y;

        for (int a = -1; a < 2; a++) {
            for (int b = -1; b < 2; b++) {
                if (a == 0 && b == 0) continue;
                if (y + a < 0 || y + a >= size || x + b < 0 || x + b >= size) continue;
                if (board[y + a][x + b] != nullptr && board[y + a][x + b]->color == figure->color) continue;

                mvs.push_back({y + a, x + b});
            }
        }

        return mvs;
    }

    vector<Move> makeFigureMoves(Figure* figure) const {
        vector<pos> mvs;

        switch (figure->type) {
        case TYPES_ROOK:
            mvs = makeRookMoves(figure);
            break;

        case TYPES_KNIGHT:
            mvs = makeKnightMoves(figure);
            break;

        case TYPES_BISHEP:
            mvs = makeBishepMoves(figure);
            break;

        case TYPES_PAWN:
            mvs = makePawnMoves(figure);
            break;

        case TYPES_KING:
            mvs = makeKingMoves(figure);
            break;
        }

        vector<Move> ans;
        ans.reserve(mvs.size());

        for (const auto& mv: mvs) {
            ans.push_back(Move{
                .coordsFrom = figure->getCoords(),
                .coordsTo = mv,
                .attacker = figure,
                .victim = board[mv.first][mv.second]
            });
        }

        return ans;
    }

    void makeRandomMove() {
        random_shuffle(figures[colorMove].begin(), figures[colorMove].end());
        vector<Move> mvs;

        for (const auto& figure: figures[colorMove]) {
            mvs = makeFigureMoves(figure);

            if (mvs.size() > 0) {
                Move mv = mvs[rand() % mvs.size()];
                cout << mv.notation() << endl;

                int cost = makeValidMove(mv);
                if (cost != -1) {
                    return;
                }
            }
        }
    }

    int makeValidMove(const Move& mv) {
        int cost = 0;

        if (board[mv.coordsTo.first][mv.coordsTo.second] != nullptr) {
            cost = board[mv.coordsTo.first][mv.coordsTo.second]->cost;

            auto delFigure = board[mv.coordsTo.first][mv.coordsTo.second];
            auto it = find(figures[delFigure->color].begin(), figures[delFigure->color].end(), delFigure);

            if (it != figures[delFigure->color].end()) {
                figures[delFigure->color].erase(it);
            }
        }

        if (cost == kings[mv.attacker->color]->cost) {
            return -1;
        }

        Figure* figure = board[mv.coordsFrom.first][mv.coordsFrom.second];

        board[figure->y][figure->x] = nullptr;
        figure->y = mv.coordsTo.first;
        figure->x = mv.coordsTo.second;
        board[figure->y][figure->x] = figure;

        history.push_back(mv);

        if (checkCheck(mv.attacker->color)) {
            unmakeMove();
            return -1;
        }

        if (colorMove == COLORS_WHITE) {
            colorMove = COLORS_BLACK;
        } else {
            colorMove = COLORS_WHITE;
        }

        COLORS victimColor = COLORS_BLACK;
        if (mv.attacker->color == victimColor) {
            victimColor = COLORS_WHITE;
        }

        if (checkCheck(victimColor)) {
            history[history.size()-1].check = true;
            return kings[victimColor]->cost;
        }

        return cost;
    }

    void unmakeMove() {
        if (history.size() <= 0) {
            return;
        }

        const Move lastMove = history[history.size()-1];

        board[lastMove.coordsFrom.first][lastMove.coordsFrom.second] = lastMove.attacker;
        lastMove.attacker->y = lastMove.coordsFrom.first;
        lastMove.attacker->x = lastMove.coordsFrom.second;
        board[lastMove.coordsTo.first][lastMove.coordsTo.second] = lastMove.victim;

        if (lastMove.victim != nullptr) {
            figures[lastMove.victim->color].push_back(lastMove.victim);
        }

        history.pop_back();
    }

    bool checkTypeCheck(Figure* figure) const {
        const auto mvs = makeFigureMoves(figure);
        for (const auto& mv: mvs) {
            if (mv.victim != nullptr && mv.victim->type == figure->type) {
                return true;
            }
        }
        return false;
    }

    bool checkCheck(COLORS color) const {
        Figure* virtualAttacker = new Rook(color, kings.at(color)->y, kings.at(color)->x);
        if (checkTypeCheck(virtualAttacker)) {
            delete virtualAttacker;
            return true;
        }
        delete virtualAttacker;

        virtualAttacker = new Bishep(color, kings.at(color)->y, kings.at(color)->x);
        if (checkTypeCheck(virtualAttacker)) {
            delete virtualAttacker;
            return true;
        }
        delete virtualAttacker;

        virtualAttacker = new Knight(color, kings.at(color)->y, kings.at(color)->x);
        if (checkTypeCheck(virtualAttacker)) {
            delete virtualAttacker;
            return true;
        }
        delete virtualAttacker;

        if (checkTypeCheck(kings.at(color))) {
            return true;
        }

        int pawnDirection = -1;
        if (color == COLORS_WHITE) {
            pawnDirection *= -1;
        }

        int pawnY = kings.at(color)->y - pawnDirection;
        if (pawnY >= size || pawnY < 0) {
            return false;
        }

        if (kings.at(color)->x > 0) {
            virtualAttacker = board[pawnY][kings.at(color)->x - 1];

            if (virtualAttacker != nullptr && virtualAttacker->type == TYPES_PAWN && virtualAttacker->color != color) {
                return true;
            }
        }

        if (kings.at(color)->x < size - 1) {
            virtualAttacker = board[pawnY][kings.at(color)->x + 1];

            if (virtualAttacker != nullptr && virtualAttacker->type == TYPES_PAWN && virtualAttacker->color != color) {
                return true;
            }
        }

        return false;
    }

    void print() const {
        cout << ' ';
        for (int i = 0; i < size; ++i) {
            cout << ' ' << char(i + 'a');
        }
        cout << endl;

        for (int i = size - 1; i >= 0; i--) {
            cout << i + 1;
            for (int j = 0; j < size; j++) {

                if (board[i][j] == nullptr) {
                    cout << "| ";
                }
                else {
                    cout << "|" << char(SYMBOLS[board[i][j]->type] + (board[i][j]->color == COLORS_WHITE ? 0 : 'a' - 'A'));
                }
            }

            cout << "|" << endl;
        }
    }
};

void findMoves(Board& b, int dep) {
    int curBest = 0;
    vector<Move> history(dep);
    map<int, vector<Move>> bestHistories;

    function<void(Board& b, int curDep, int sum)> rec;
    rec = [dep, &curBest, &history, &rec, &bestHistories](Board& b, int curDep, int sum) {
        for (const auto& fig: b.figures[COLORS_WHITE]) {
            const auto mvs = b.makeFigureMoves((fig));

            for (const auto& mv: mvs) {
                int cost = b.makeValidMove(mv);
                if (cost == -1) {
                    continue;
                }

                curBest = (sum + cost);

                if (b.history[b.history.size()-1].check || curDep + 1 == dep) {
                    history.resize(curDep + 1);
                    copy(prev(b.history.end(), curDep + 1), b.history.end(), history.begin());

                    bestHistories.insert({curBest, history});

                    if (bestHistories.size() > dep * 2) {
                        bestHistories.erase(bestHistories.begin());
                    }
                } else {
                    rec(b, curDep + 1, sum + cost);
                }

                b.unmakeMove();
            }
        }
    };

    rec(b, 0, 0);

    for (const auto& pr: bestHistories) {
        cout << pr.first << endl;

        for (const auto& mv: pr.second) {
            cout << mv.notation() << endl;
            b.makeValidMove(mv);
        }
        b.print();

        for (const auto& mv: pr.second) {
            b.unmakeMove();
        }

        cout << endl;
    }
}

int main() {
    Board board;
    board.print();
    cout << endl;

    srand(time(0));

    int movesCount;
    cout << "Enter moves count: ";
    cin >> movesCount;

    cout << "Game moves:" << endl;
    for (int i = 0; i < movesCount; ++i) {
        board.makeRandomMove();
    }
    cout << endl;

    board.print();
    cout << endl;

    findMoves(board, 3);

    return 0;
}
