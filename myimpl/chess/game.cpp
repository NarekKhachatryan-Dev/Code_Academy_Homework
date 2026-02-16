#include "game.h"
#include <iostream>

Game::Game() : whiteTurn(true), isRunning(true) {}

bool Game::parseInput(const std::string& input, int& row, int& col) {
    if (input.length() != 2) return false;

    col = input[0] - 'a';
    row = 8 - (input[1] - '0');

    return (row >= 0 && row < 8 && col >= 0 && col < 8);
}

void Game::start() {
    std::cout << "Welcome to Chess!" << std::endl;
    std::cout << "Enter moves in format: e2 e4" << std::endl;

    while (isRunning) {
        board.printChessboard();
        
        bool inCheck = board.isCheck(whiteTurn);
        
        if (inCheck) {
            if (board.isCheckmate(whiteTurn)) {
                std::cout << "CHECKMATE! " << (whiteTurn ? "Black" : "White") << " wins!" << std::endl;
                break;
            }
            std::cout << "CHECK!" << std::endl;
        } else {
            if (board.isStalemate(whiteTurn)) {
                std::cout << "STALEMATE! It's a draw." << std::endl;
                break;
            }
        }

        std::cout << (whiteTurn ? "White's" : "Black's") << " turn: ";
        handleInput();
    }
}

void Game::handleInput() {
    std::string moveFrom, moveTo;
    if (!(std::cin >> moveFrom >> moveTo)) {
        isRunning = false;
        return;
    }

    int r1, c1, r2, c2;
    if (!parseInput(moveFrom, r1, c1) || !parseInput(moveTo, r2, c2)) {
        std::cout << "Invalid format! Use e2 e4." << std::endl;
        return;
    }

    const auto& p = board.getElement(r1, c1);
    if (p && p->isWhite() == whiteTurn) {
        char promotionPiece = 'q';

        char symbol = std::tolower(p->getSymbol());
        if (symbol == 'p' && (r2 == 0 || r2 == 7)) {
            std::cout << "Promote to (q, r, b, n): ";
            std::cin >> promotionPiece;
        }

        if (board.makeMove(r1, c1, r2, c2, promotionPiece)) {
            whiteTurn = !whiteTurn;
        } else {
            std::cout << "Invalid move! Try again." << std::endl;
        }
    } else {
        std::cout << "It's not your piece!" << std::endl;
    }
}