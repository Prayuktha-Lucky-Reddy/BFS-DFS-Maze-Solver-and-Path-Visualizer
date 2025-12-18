#include <iostream>
#include <windows.h>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

const int DEFAULT_N = 5;
const int MIN_N = 2;
const int SLEEP_MS = 500;

void sleep_step() {
    Sleep(SLEEP_MS);
}

void print_grid(const vector<vector<char>>& grid, int sr, int sc, int er, int ec) {
    // Clear screen (simple)
    cout << string(40, '\n');
    int n = grid.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            char ch = grid[i][j];
            if (i == sr && j == sc) ch = 'S';
            if (i == er && j == ec) ch = 'E';
            cout << ch << ' ';
        }
        cout << '\n';
    }
    cout.flush();
}

bool parse_four_ints(const string& line, int& a, int& b, int& c, int& d) {
    stringstream ss(line);
    if (!(ss >> a >> b >> c >> d)) return false;
    return true;
}

bool parse_two_ints(const string& line, int& a, int& b) {
    stringstream ss(line);
    if (!(ss >> a >> b)) return false;
    return true;
}

bool in_bounds(int r, int c, int n) {
    return r >= 0 && r < n && c >= 0 && c < n;
}

// DFS visualization
void run_dfs(vector<vector<char>> grid, int sr, int sc, int er, int ec) {
    int n = grid.size();
    vector<vector<int>> visited(n, vector<int>(n, 0));
    bool found = false;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    // iterative DFS stack: (r, c, next_dir)
    struct Node { int r, c, dir; };
    vector<Node> stack;

    if (!in_bounds(sr, sc, n) || !in_bounds(er, ec, n)) {
        cout << "Invalid start or end.\n";
        sleep_step();
        return;
    }
    if (grid[sr][sc] == '#' || grid[er][ec] == '#') {
        cout << "Start or end is a wall.\n";
        sleep_step();
        return;
    }

    stack.push_back({sr, sc, 0});
    visited[sr][sc] = 1;

    while (!stack.empty()) {
        Node &cur = stack.back();
        int r = cur.r;
        int c = cur.c;

        // mark visited for visualization
        if (!(r == sr && c == sc) && !(r == er && c == ec) && grid[r][c] == '.')
            grid[r][c] = 'V';

        print_grid(grid, sr, sc, er, ec);
        sleep_step();

        if (r == er && c == ec) {
            found = true;
            break;
        }

        if (cur.dir == 4) {
            stack.pop_back();
            continue;
        }

        int d = cur.dir;
        cur.dir++;
        int nr = r + dr[d];
        int nc = c + dc[d];

        if (in_bounds(nr, nc, n) && !visited[nr][nc] && grid[nr][nc] != '#') {
            visited[nr][nc] = 1;
            stack.push_back({nr, nc, 0});
        }
    }

    if (!found) {
        cout << "DFS: No path found.\n";
        sleep_step();
    } else {
        cout << "DFS: Reached end.\n";
        sleep_step();
    }
}

// BFS visualization with shortest path
void run_bfs(vector<vector<char>> grid, int sr, int sc, int er, int ec) {
    int n = grid.size();
    vector<vector<int>> visited(n, vector<int>(n, 0));
    vector<vector<pair<int,int>>> parent(n, vector<pair<int,int>>(n, make_pair(-1,-1)));

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    if (!in_bounds(sr, sc, n) || !in_bounds(er, ec, n)) {
        cout << "Invalid start or end.\n";
        sleep_step();
        return;
    }
    if (grid[sr][sc] == '#' || grid[er][ec] == '#') {
        cout << "Start or end is a wall.\n";
        sleep_step();
        return;
    }

    queue<pair<int,int>> q;
    q.push(make_pair(sr, sc));
    visited[sr][sc] = 1;

    bool found = false;

    while (!q.empty()) {
        pair<int,int> cur = q.front();
        q.pop();
        int r = cur.first;
        int c = cur.second;

        if (!(r == sr && c == sc) && !(r == er && c == ec) && grid[r][c] == '.')
            grid[r][c] = 'V';

        print_grid(grid, sr, sc, er, ec);
        sleep_step();

        if (r == er && c == ec) {
            found = true;
            break;
        }

        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k];
            int nc = c + dc[k];
            if (in_bounds(nr, nc, n) && !visited[nr][nc] && grid[nr][nc] != '#') {
                visited[nr][nc] = 1;
                parent[nr][nc] = make_pair(r, c);
                q.push(make_pair(nr, nc));
            }
        }
    }

    if (!found) {
        cout << "BFS: No path found.\n";
        sleep_step();
        return;
    }

    // reconstruct path
    int r = er, c = ec;
    while (!(r == sr && c == sc)) {
        if (!(r == sr && c == sc) && !(r == er && c == ec))
            grid[r][c] = '*';
        pair<int,int> p = parent[r][c];
        r = p.first;
        c = p.second;
    }

    print_grid(grid, sr, sc, er, ec);
    cout << "BFS: Shortest path shown with '*'.\n";
    sleep_step();
}

int main() {
    int n = DEFAULT_N;
    vector<vector<char>> grid(n, vector<char>(n, '.'));
    int sr = 0, sc = 0;
    int er = n - 1, ec = n - 1;

    while (true) {
        cout << string(5, '\n');
        cout << "Current grid size: " << n << " x " << n << "\n";
        print_grid(grid, sr, sc, er, ec);
        cout << "Menu:\n";
        cout << "1. Increase size\n";
        cout << "2. Decrease size\n";
        cout << "3. Add wall\n";
        cout << "4. Remove wall\n";
        cout << "5. Run DFS\n";
        cout << "6. Run BFS\n";
        cout << "7. Quit\n";
        cout << "Enter choice: ";

        int choice;
        if (!(cin >> choice)) break;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 7) break;

        if (choice == 1) {
            n += 1;
            grid.assign(n, vector<char>(n, '.'));
            sr = 0; sc = 0;
            er = n - 1; ec = n - 1;
        } else if (choice == 2) {
            if (n > MIN_N) {
                n -= 1;
                grid.assign(n, vector<char>(n, '.'));
                sr = 0; sc = 0;
                er = n - 1; ec = n - 1;
            } else {
                cout << "Minimum size is " << MIN_N << ".\n";
                sleep_step();
            }
        } else if (choice == 3) {
            cout << "Enter wall coordinates (row col): ";
            string line;
            getline(cin, line);
            int r, c;
            if (!parse_two_ints(line, r, c)) {
                cout << "Invalid input.\n";
                sleep_step();
            } else if (!in_bounds(r, c, n)) {
                cout << "Out of bounds.\n";
                sleep_step();
            } else {
                if (!((r == sr && c == sc) || (r == er && c == ec)))
                    grid[r][c] = '#';
            }
        } else if (choice == 4) {
            cout << "Enter wall coordinates to remove (row col): ";
            string line;
            getline(cin, line);
            int r, c;
            if (!parse_two_ints(line, r, c)) {
                cout << "Invalid input.\n";
                sleep_step();
            } else if (!in_bounds(r, c, n)) {
                cout << "Out of bounds.\n";
                sleep_step();
            } else {
                if (grid[r][c] == '#') grid[r][c] = '.';
            }
        } else if (choice == 5 || choice == 6) {
            cout << "Enter start and end (sr sc er ec), or just press Enter for defaults: ";
            string line;
            getline(cin, line);
            int tsr = sr, tsc = sc, ter = er, tec = ec;
            if (!line.empty()) {
                if (!parse_four_ints(line, tsr, tsc, ter, tec)) {
                    cout << "Invalid input, using defaults.\n";
                    sleep_step();
                    tsr = sr; tsc = sc; ter = er; tec = ec;
                }
            }
            if (choice == 5) run_dfs(grid, tsr, tsc, ter, tec);
            else run_bfs(grid, tsr, tsc, ter, tec);
        } else {
            cout << "Invalid choice.\n";
            sleep_step();
        }
    }

    return 0;
}
