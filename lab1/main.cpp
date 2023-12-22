#include <iostream>
#include <vector>
#include <map>

using namespace std;

typedef pair<int, int> Pos;
typedef pair<Pos, Pos> Move;

enum COLOR {
	COLOR_WHITE,
	COLOR_BLACK,
	COLOR_UNDEFINED,
};

class Board {
	public:
	int size;
	vector<Figure*> white;
	vector<Figure*> black;
	vector<vector<Figure*>> board;
	map<Pos, bool> whiteMateMoves;

	Board(vector<Figure*> white, vector<Figure*> black) {
		size = 8;
		this->white = white;
		this->black = black;

		for (int i = 0; i < size; i++) {

			board.resize(size);
			for (int j = 0; j < size; j++) {
				board[i][j] = nullptr;
			}
		}

		for (auto figure : white) {
			board[figure->p.first][figure->p.second] = figure;
		}

		for (auto figure : black) {
			board[figure->p.first][figure->p.second] = figure;
		}

		initMateMoves();
	}

	bool isValidMove(Pos p) {
		return 0 <= p.first && p.first < size && 0 <= p.second && p.second <= size;
	}

	int getCost(Pos p) {
		if (!isValidMove(p)) return -1;
		if (board[p.first][p.second] = nullptr) return 0;
		return board[p.first][p.second]->cost;
	}

	bool isNotColor(Pos p, COLOR color) {
		if (getCost(p) <= 0) return 0;
		return board[p.first][p.second]->color != color;
	}

	void initMateMoves() {
		auto& attackers = black;

		for (auto& figure : attackers) {
			auto moves = figure->getAttacks(this);
			for (auto& move : moves) {

				whiteMateMoves[move.first.second] = 1;
			}
		}
	}
};

class Figure {
	public:
	int cost;
	Pos p;
	COLOR color;

	virtual vector<pair<Move, int>> getMoves(Board* board);
	virtual vector<pair<Move, int>> getAttacks(Board* board);

	Figure(Pos p, COLOR color) {
		this->p = p;
		this->color = color;
	}

	Move makeMove(Pos p1) {
		return make_pair(make_pair(p.first, p.second), make_pair(p1.first, p1.second));
	}
};

class Pawn : Figure {
	public:
	int cost = 1;

	Pawn(Pos p, COLOR color) : Figure(p, color) {}

	vector<pair<Move, int>> getAttacks(Board* board) override {
		vector<pair<Move, int>> moves;
		int s = 1;
		if (color == COLOR_BLACK) s = -1;

		for (int i = 1; i < 3; i++) {

			if (i == 2 && (p.second - 2 * s) % board->size != 0) continue;

			Pos p1 = p;
			p1.second += i * s;

			if (board->getCost(p1) == 0) {
				moves.push_back(make_pair(makeMove(p1), 0));
			}
		}

		Pos p1(p.first + 1, p.second + 1 * s);
		if (board->getCost(p1) > 0 && board->isNotColor(p1, color)) {
			moves.push_back(make_pair(makeMove(p1), board->getCost(p1)));
		}
		p1 = Pos(p.first - 1, p.second + 1 * s);
		if (board->getCost(p1) > 0 && board->isNotColor(p1, color)) {
			moves.push_back(make_pair(makeMove(p1), board->getCost(p1)));
		}

		return moves;
	}

	vector<pair<Move, int>> getMoves(Board* board) override {
		vector<pair<Move, int>> moves;
		int s = 1;
		if (color == COLOR_BLACK) s = -1;

		Pos p1(p.first + 1, p.second + 1 * s);
		if (board->getCost(p1) > 0 && board->isNotColor(p1, color)) {
			moves.push_back(make_pair(makeMove(p1), board->getCost(p1)));
		}
		p1 = Pos(p.first - 1, p.second + 1 * s);
		if (board->getCost(p1) > 0 && board->isNotColor(p1, color)) {
			moves.push_back(make_pair(makeMove(p1), board->getCost(p1)));
		}

		return moves;
	}
};

class Rook : Figure {
	public:
	int cost = 5;

	Rook(Pos p, COLOR color) : Figure(p, color) {}

	vector<pair<Move, int>> getMoves(Board* board) override {
		vector<pair<Move, int>> moves;

		// up
		for (int i = p.second + 1; i < board->size; i++) {

			Pos p1(p.first, i);
			int curCost = board->getCost(p1);

			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		// down
		for (int i = p.second - 1; i >= 0; i--) {

			Pos p1(p.first, i);
			int curCost = board->getCost(p1);

			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		// right
		for (int i = p.first + 1; i < board->size; i++) {

			Pos p1(i, p.second);
			int curCost = board->getCost(p1);

			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		// left
		for (int i = p.first - 1; i >= 0; i--) {

			Pos p1(i, p.second);
			int curCost = board->getCost(p1);

			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		return moves;
	}

	vector<pair<Move, int>> getAttacks(Board* board) override {
		return getMoves(board);
	}
};

class Bishep : Figure {
	public:
	int cost = 3;

	Bishep(Pos p, COLOR color) : Figure(p, color) {}

	vector<pair<Move, int>> getMoves(Board* board) override {
		vector<pair<Move, int>> moves;

		// up right
		for (
			auto p1 = Pos(p.first + 1, p.second + 1);
			board->isValidMove(p1);
			p1 = Pos(p1.first + 1, p1.second + 1)
			) {

			int curCost = board->getCost(p1);
			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		// down right
		for (
			auto p1 = Pos(p.first + 1, p.second - 1);
			board->isValidMove(p1);
			p1 = Pos(p1.first + 1, p1.second - 1)
			) {

			int curCost = board->getCost(p1);
			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		// up left
		for (
			auto p1 = Pos(p.first - 1, p.second + 1);
			board->isValidMove(p1);
			p1 = Pos(p1.first - 1, p1.second + 1)
			) {

			int curCost = board->getCost(p1);
			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		// down left
		for (
			auto p1 = Pos(p.first - 1, p.second - 1);
			board->isValidMove(p1);
			p1 = Pos(p1.first - 1, p1.second - 1)
			) {

			int curCost = board->getCost(p1);
			if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

				moves.push_back(make_pair(makeMove(p1), curCost));
			}

			if (curCost != 0) {
				break;
			}
		}

		return moves;
	}

	vector<pair<Move, int>> getAttacks(Board* board) override {
		return getMoves(board);
	}
};

class Knight : Figure {
	public:
	int cost = 3;

	Knight(Pos p, COLOR color) : Figure(p, color) {}

	vector<pair<Move, int>> getMoves(Board* board) override {
		vector<pair<Move, int>> moves;

		for (int s1 = -1; s1 < 2; s1 += 2) {
			for (int s2 = -1; s2 < 2; s2 += 2) {

				Pos p1(2 * s1, 1 * s2);
				int curCost = board->getCost(p1);

				if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

					moves.push_back(make_pair(makeMove(p1), curCost));
				}

				p1 = Pos(1 * s1, 2 * s2);
				curCost = board->getCost(p1);

				if (curCost == 0 || (curCost > 0 && board->isNotColor(p1, color))) {

					moves.push_back(make_pair(makeMove(p1), curCost));
				}
			}
		}

		return moves;
	}

	vector<pair<Move, int>> getAttacks(Board* board) override {
		return getMoves(board);
	}
};

class Queen : Figure {
	private:
	Rook* rook = nullptr;
	Bishep* bishep = nullptr;

	public:
	int cost = 9;

	Queen(Pos p, COLOR color) : Figure(p, color) {
		rook = new Rook(p, color);
		bishep = new Bishep(p, color);
	}

	vector<pair<Move, int>> getMoves(Board* board) override {
		vector<pair<Move, int>> moves = rook->getMoves(board);
		vector<pair<Move, int>> moves1 = bishep->getMoves(board);

		moves.insert(moves.end(), moves1.begin(), moves1.end());

		return moves;
	}

	vector<pair<Move, int>> getAttacks(Board* board) override {
		return getMoves(board);
	}
};

class King : Figure {
	public:
	int cost = 100;

	King(Pos p, COLOR color) : Figure(p, color) {}

	vector<pair<Move, int>> getMoves(Board* board) override {
		vector<pair<Move, int>> moves;

		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {

				if (x == 0 && y == 0) continue;

				Pos p1(p.first + x, p.second + y);
				int curCost = board->getCost(p1);

				if (!board->whiteMateMoves[p1]) {
					moves.push_back(make_pair(makeMove(p1), curCost));
				}

			}
		}

		return moves;
	}

	vector<pair<Move, int>> getAttacks(Board* board) override {
		return getMoves(board);
	}
};

int main() {
	vector<Figure*> white, black;
	int n;

	cout << "Enter figure counts\n";
	cout << "Pawn white (0-8): ";
	cin >> n;


	cout << "Pawn black (0-8): ";
	cin >> n;


	cout << "Rook white (0-2): ";
	cin >> n;


	cout << "Rook black (0-2): ";
	cin >> n;


	cout << "Bishep white (0-2): ";
	cin >> n;


	cout << "Bishep black (0-2): ";
	cin >> n;


	cout << "Queen white (0-1): ";
	cin >> n;


	cout << "Queen black (0-1): ";
	cin >> n;



	return 0;
}
