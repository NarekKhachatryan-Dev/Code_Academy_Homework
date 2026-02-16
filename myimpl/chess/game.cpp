#include "game.h"
#include <iostream>
#include <stdexcept>

Game::Game() : whiteTurn(true), isRunning(true) {}

bool Game::parseInput(const std::string& input, int& row, int& col) {
    if (input.length() != 2) {
        throw std::invalid_argument("Input must be 2 characters (e.g., e2)");
    }
    col = input[0] - 'a';
    row = 8 - (input[1] - '0');
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        throw std::out_of_range("Coordinates out of chess board");
    }
    return true;
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

void Game::loadAllTextures() {
    std::string pieces = "pnbrqkPNBRQK"; 
    
    for (char c : pieces) {
        sf::Texture tex;
        std::string filename = "images/";
        filename += c;
        filename += ".png";

        if (!tex.loadFromFile(filename)) {
            std::cerr << "Error loading texture: " << filename << std::endl;
        } else {
            textureMap[c] = std::move(tex);
        }
    }
}

void Game::runGUI() {
sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");
loadAllTextures();

bool isPieceSelected = false;
int fromR = -1, fromC = -1;

while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                int c = event.mouseButton.x / 100;
                int r = event.mouseButton.y / 100;

                if (!isPieceSelected) {
                    char s = board.getPieceSymbol(r, c);
                    if (s != '.') {
                        bool isWhitePiece = std::isupper(s);
                        if (isWhitePiece == whiteTurn) {
                            fromR = r;
                            fromC = c;
                            isPieceSelected = true;
                        }
                    }
                } else {
                    if (board.makeMove(fromR, fromC, r, c, 'q')) {
                        whiteTurn = !whiteTurn;

                        if (board.isCheck(whiteTurn)) {
                            if (board.isCheckmate(whiteTurn)) {
                                board.printChessboard();
                                std::cout << "CHECKMATE! " << (whiteTurn ? "Black" : "White") << " wins!" << std::endl;
                            } else {
                                std::cout << "CHECK!" << std::endl;
                            }
                        } else if (board.isStalemate(whiteTurn)) {
                            std::cout << "STALEMATE! It's a draw." << std::endl;
                        }
                    } else {
                        std::cout << "Invalid move!" << std::endl;
                    }
                    isPieceSelected = false;
                }
            }
        }
    }

    window.clear();

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            sf::RectangleShape rect(sf::Vector2f(100.f, 100.f));
            rect.setPosition(c * 100.f, r * 100.f);
            
            if (isPieceSelected && r == fromR && c == fromC) {
                rect.setFillColor(sf::Color(255, 255, 150));
            } else {
                rect.setFillColor((r + c) % 2 == 0 ? sf::Color(238, 238, 210) : sf::Color(118, 150, 86));
            }
            window.draw(rect);

            char symbol = board.getPieceSymbol(r, c);
            if (symbol != '.') {
                sf::Sprite sprite;
                sprite.setTexture(textureMap[symbol]);
                sprite.setPosition(c * 100.f, r * 100.f);
                window.draw(sprite);
            }
        }
    }
    window.display();
}
}

void Game::drawBoard(sf::RenderWindow& window) {
for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
        sf::RectangleShape rect(sf::Vector2f(100.f, 100.f));
        rect.setPosition(c * 100.f, r * 100.f);
        rect.setFillColor((r + c) % 2 == 0 ? sf::Color(238, 238, 210) : sf::Color(118, 150, 86));
        window.draw(rect);

        char symbol = board.getPieceSymbol(r, c);
        if (symbol != '.') {
            sf::Sprite sprite;
            sprite.setTexture(textureMap[symbol]);
            sprite.setPosition(c * 100.f, r * 100.f);
            window.draw(sprite);
        }
    }
}
}

void Game::handleInput() {
    std::string moveFrom, moveTo;
    
    try {
        if (!(std::cin >> moveFrom >> moveTo)) {
            isRunning = false;
            return;
        }

        int r1, c1, r2, c2;
        parseInput(moveFrom, r1, c1);
        parseInput(moveTo, r2, c2);

        const auto& p = board.getElement(r1, c1);
    
        if (!p) {
            throw std::runtime_error("There is no piece at the starting position!");
        }
        
        if (p->isWhite() != whiteTurn) {
            throw std::runtime_error("It's not your turn or not your piece!");
        }

        char promotionPiece = 'q';
        char symbol = std::tolower(p->getSymbol());
        
        if (symbol == 'p' && (r2 == 0 || r2 == 7)) {
            std::cout << "Promote to (q, r, b, n): ";
            if (!(std::cin >> promotionPiece)) return;
        }

        if (board.makeMove(r1, c1, r2, c2, promotionPiece)) {
            whiteTurn = !whiteTurn;
        } else {
            throw std::logic_error("The move is rule-breaking (blocked path or puts King in check).");
        }

    } catch (const std::invalid_argument& e) {
        std::cout << "Format Error: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Board Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Game Error: " << e.what() << std::endl;
    }
}