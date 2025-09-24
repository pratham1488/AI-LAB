#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

int n = 3;

struct State {
    State* parent;
    vector<vector<char>> board;
    int score;
    bool isMax;                // true = X's turn, false = O's turn
    vector<State*> children;   // possible next moves
};

// ---------- Utility ----------
bool moves_left(const vector<vector<char>>& board) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (board[i][j] == '_')
                return true;
    return false;
}

int get_score(const vector<vector<char>>& board) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != '_' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0] == 'X' ? 1 : -1;
        if (board[0][i] != '_' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i] == 'X' ? 1 : -1;
    }
    if (board[0][0] != '_' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0] == 'X' ? 1 : -1;
    if (board[0][2] != '_' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2] == 'X' ? 1 : -1;
    return 0;
}

void print_board(const vector<vector<char>>& board) {
    cout << "\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << board[i][j] << " ";
        cout << "\n";
    }
    cout << "\n";
}

// ---------- Tree generation ----------
int generate_tree(State* s) {
    int score = get_score(s->board);
    if (score != 0) {
        s->score = score;
        return score;
    }
    if (!moves_left(s->board)) {
        s->score = 0;
        return 0;
    }

    if (s->isMax) { // X's move
        int best = INT_MIN;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (s->board[i][j] == '_') {
                    auto newBoard = s->board;
                    newBoard[i][j] = 'X';
                    State* child = new State{s, newBoard, 0, false, {}};
                    s->children.push_back(child);
                    best = max(best, generate_tree(child));
                }
            }
        }
        s->score = best;
        return best;
    } else { // O's move
        int best = INT_MAX;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (s->board[i][j] == '_') {
                    auto newBoard = s->board;
                    newBoard[i][j] = 'O';
                    State* child = new State{s, newBoard, 0, true, {}};
                    s->children.push_back(child);
                    best = min(best, generate_tree(child));
                }
            }
        }
        s->score = best;
        return best;
    }
}

// ---------- Gameplay ----------
State* best_computer_move(State* s) {
    int bestVal = INT_MIN;
    State* bestChild = nullptr;
    for (auto child : s->children) {
        if (child->score > bestVal) {
            bestVal = child->score;
            bestChild = child;
        }
    }
    return bestChild;
}

void play_game(State* root) {
    State* current = root;

    while (true) {
        print_board(current->board);
        int score = get_score(current->board);

        if (score == 1) { cout << "Computer (X) wins!\n"; break; }
        if (score == -1) { cout << "You (O) win!\n"; break; }
        if (!moves_left(current->board)) { cout << "It's a draw!\n"; break; }

        // User move
        int x, y;
        cout << "Enter your move (row col, 0-indexed): ";
        cin >> x >> y;

        if (x < 0 || x >= n || y < 0 || y >= n || current->board[x][y] != '_') {
            cout << "Invalid move. Try again.\n";
            continue;
        }

        // Find the child corresponding to this move
        State* nextState = nullptr;
        for (auto child : current->children) {
            if (child->board[x][y] == 'O') {
                nextState = child;
                break;
            }
        }
        if (nextState == nullptr) {
            // If we didn’t prebuild correctly, fallback: manually set O
            auto newBoard = current->board;
            newBoard[x][y] = 'O';
            nextState = new State{current, newBoard, 0, true, {}};
            generate_tree(nextState);
            current->children.push_back(nextState);
        }

        current = nextState;

        print_board(current->board);
        score = get_score(current->board);
        if (score == -1) { cout << "You (O) win!\n"; break; }
        if (!moves_left(current->board)) { cout << "It's a draw!\n"; break; }

        // Computer move
        current = best_computer_move(current);
        cout << "Computer plays:\n";
    }
}


int main() {
    vector<vector<char>> board = {
        {'_', '_', '_'},
        {'_', '_', '_'},
        {'_', '_', '_'}
    };

    // Root: X starts (computer)
    State* root = new State{nullptr, board, 0, true, {}};
    generate_tree(root);

    // Let player be O, so skip computer’s first move if needed
    play_game(root);

    return 0;
}
