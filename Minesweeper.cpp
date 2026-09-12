#include <iostream>
#include <cstdlib>
#include <vector>
#include <windows.h>
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());

//Color variables
#define RED     "\033[31m"// Selected && Error
#define GREEN   "\033[32m"// 1
#define YELLOW  "\033[33m"// 2
#define BLUE    "\033[34m"// 3
#define MAGENTA "\033[35m"// 4
#define CYAN    "\033[36m"// 5-8
#define WHITE   "\033[37m" // 0
#define RESET   "\033[0m"

void GenerateBoard(vector<vector <char>>& board, int size_x, int size_y) { //Creates playable board depending on size
    for (int col = 0; col < size_y; col++) {
        board.push_back(vector<char>(size_x, '#'));
    }
}

bool HasWon(vector<vector <char>>& board) {
    //Loops through the hidden board
    for (int col = 0; col < board.size(); col++) {
        for (int row = 0; row < board[col].size(); row++) { 
            if (board[col][row] == '#') { //If it still has an empty square then return false
                return false;
            }
        }
    }



    return true;
}

void Move(vector<vector <char>>& board, char direction, int& selected_x, int& selected_y, int boardx, int boardy) {
    //Checks the direction, and then if it doesn't go beyond the border [WALLS]


    // Up
    if (direction == 'W' && selected_y > 0) {
        selected_y -= 1;
    } else if (direction == 'S' && selected_y < boardy - 1) { // Down
        selected_y += 1;
    } else if (direction == 'D' && selected_x < boardx - 1) { // Right
        selected_x += 1;
    } else if (direction == 'A' && selected_x > 0) { // Left
        selected_x -= 1;
    }
}

int LocateMines(vector<vector <char>>& board, int row, int col, int boardx, int boardy) {
    int v = 0; // value to return

    // Up
    if (col > 0 && board[col-1][row] == '@') { 
        v++;
    }

    //Down
    if (col < boardy-1 && board[col+1][row] == '@') {
        v++;
    }

    // Left
    if (row > 0 && board[col][row-1] == '@') {
        v++;
    }

    // Right
    if (row < boardx-1 && board[col][row+1] == '@') {
        v++;
    }

    // Up Right
    if (col > 0 && row < boardx-1 && board[col-1][row+1] == '@') { 
        v++;
    }

    // Up Left
    if (col > 0 && row > 0 && board[col-1][row-1] == '@') { 
        v++;
    }

    // Down Right
    if (col < boardy-1 && row < boardx-1 && board[col+1][row+1] == '@') { 
        v++;
    }

    // Down Left
    if (col < boardy-1 && row > 0 && board[col+1][row-1] == '@') { 
        v++;
    }

    return v;
}

void Reveal(vector<vector <char>>& board,vector<vector <char>>& Pboard, int selected_x, int selected_y, bool& alive, int bx, int by, int& af) {
    char x = '0';

    vector<pair<int, int>> Position = {
        {-1, -1},  {-1, 0},  {-1, +1},
        { 0, -1},            { 0, +1},
        {+1, -1},  {+1, 0},  {+1, +1},
    };

    if (Pboard[selected_y][selected_x] == 'F') {
        Pboard[selected_y][selected_x] = '*';
        af++;
    }

    if (Pboard[selected_y][selected_x] != '*') {
        return;
    }

    if (board[selected_y][selected_x] == '@') { // The player revealed a mine, and dies.

        Pboard[selected_y][selected_x] = '@';

        alive = false;
        return;

    } else {
        x = x + LocateMines(board, selected_x, selected_y, bx, by);
        Pboard[selected_y][selected_x] = x;
        board[selected_y][selected_x] = x;

        if (x == '0') {
            for (int i = 0; i < Position.size(); i++) {
                int new_x = selected_x + Position[i].first;
                int new_y = selected_y + Position[i].second;

                
                if (new_x >= 0 && new_x < bx && new_y >= 0 && new_y < by) {
                    Reveal(board, Pboard, new_x, new_y, alive, bx, by, af);
                }
            }
        }
    }



    
}

void GenerateMines(vector<vector <char>>& board, int target, int boardx, int boardy) {
    //Initializes a random position;
    int random_x;
    int random_y;

    uniform_int_distribution<int> distx(0, boardx-1); //Initializes the random distance;
    uniform_int_distribution<int> disty(0, boardy-1);

    int amount_mines_placed = 0; // Condition

    while (amount_mines_placed < target) { // Target being amount_of_mines
        random_x = distx(gen);
        random_y = disty(gen);

        if (board[random_y][random_x] == '#') {
            board[random_y][random_x] = '@'; // Turns the empty space into a mine.
            amount_mines_placed++;
        }
    }
    
}

void PlacesFlag(vector<vector <char>>& Pboard, int selected_x, int selected_y, int& amount_flags) {
    if (Pboard[selected_y][selected_x] == '*' && amount_flags > 0) {
        Pboard[selected_y][selected_x] = 'F';

        amount_flags -= 1;

    } else if (Pboard[selected_y][selected_x] == 'F') { // Removes the flag if the selected position is a flag.
        Pboard[selected_y][selected_x] = '*';
        amount_flags += 1;

    } else { // Error validation
        std::cout << RED << "Tried Planting a Flag on revealed mine or does not have enough flags" << RESET;
        Sleep(1500);
    }
    
}

void HideBoard(vector<vector <char>>& Pboard) {
    for (int col = 0; col < Pboard.size(); col++) {

        for (int row = 0; row < Pboard[col].size(); row++) {
            if (Pboard[col][row] != '*') {
                Pboard[col][row] = '*';
            }
        }
    }
}

void DrawBoard(vector<vector <char>>& board, int selected_x, int selected_y) { //Displays the PlayerBoard


    for (int col = 0; col < board.size(); col++) {
        

        for (int row = 0; row < board[col].size(); row++) {
            
            if (col == selected_y && row == selected_x) {
                std::cout << RED << board[col][row] << RESET; //Selected Piece
            } else if (board[col][row] == 'F') { //Flags
                std::cout << RED << board[col][row] << RESET;
            }

            //Displays for every number
            else if (board[col][row] == '1') {
                std::cout << GREEN << board[col][row] << RESET;
            } else if (board[col][row] == '2') {
                std::cout << YELLOW << board[col][row] << RESET;
            } else if (board[col][row] == '3') {
                std::cout << BLUE << board[col][row] << RESET;
            } else if (board[col][row] == '4') {
                std::cout << MAGENTA << board[col][row] << RESET;
            } else if (board[col][row] >= '5' && board[col][row] <= '8') {
                std::cout << CYAN << board[col][row] << RESET;
            } else {
                std::cout << board[col][row];
            }
        


        }

        std::cout << endl;
    }
}

int main() {
    //Game Variables
    char Difficulty; // Increases board size, and amount of mines
    int board_size_x;
    int board_size_y;

    int selected_x = 0; // Position of the selected char [x]
    int selected_y = 0; // Position of the selected char [y]

    int amount_of_mines;
    int amount_of_flags;

    char action;

    bool IsAlive = true;

    //System Board (Contains everything revealed)
    vector<vector <char>> board;
    vector<vector <char>> PlayerBoard; // Contains just what the player looked.


    std::cout << "Please select a difficulty [e = Easy, m = Medium, h = Hard]: ";
    std::cin >> Difficulty;

    if (Difficulty == 'e') {
        board_size_x = 9;
        board_size_y = 9;

        amount_of_mines = 10;
        amount_of_flags = amount_of_mines;
    } else {
        return 0;
    }

    GenerateBoard(board, board_size_x, board_size_y);
    GenerateMines(board, amount_of_mines, board_size_x, board_size_y);


    PlayerBoard = board;
    HideBoard(PlayerBoard);
    
    

    while (IsAlive) {
        system("cls");

        
        DrawBoard(PlayerBoard, selected_x, selected_y);

        std::cout << "\nCONTROLS {\n";
        std::cout << "W-A-S-D: Move to the next selected position.\n";
        std::cout << "F: Place's a flag [Current amount of flags: " << amount_of_flags << "]\n";

        std::cout << "L: Digs the spot selected\n";
        std::cout << "}\n\n";

        std::cout << "Type your action: ";
        std::cin >> action;

        if (action == 'W' || action == 'A' || action == 'S' || action == 'D') { // Action is a movement
            Move(PlayerBoard, action, selected_x, selected_y, board_size_x, board_size_y);
        } else if (action == 'F') { // Actions places flags.
            PlacesFlag(PlayerBoard, selected_x, selected_y, amount_of_flags);

        } else if (action == 'L') { // Actions digs
            Reveal(board, PlayerBoard, selected_x, selected_y, IsAlive, board_size_x, board_size_y, amount_of_flags);

        } else {
            std::cout << RED << "THIS ACTION DOES NOT EXIST" << RESET; //Gives a fake error if the action does not exist
            Sleep(2000);
        }


        if (HasWon(board)) {
            break;
        }

        
    }

    if (IsAlive == false) { //HasLost
        std::cout << "\nGAME OVER! \n";
        std::cout << "Project built by: Anastacio.\n";
        std::cout << "Thank you for playing!";
    } else { //HasWon
        std::cout << "\n YOU WON! \n";
        std::cout << "Project built by Anastacio";
        std::cout << "Thanks for playing!";
    }
    


    std::cin.get();
    std::cin.ignore();

    return 0;
}

