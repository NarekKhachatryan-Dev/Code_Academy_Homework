#include "game.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

Game::Game() {
    board.clear();
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
    sf::RenderWindow window(sf::VideoMode(800, 900), "Position Analyzer");
    loadAllTextures();
    board.clear();

    // load and remember font for labels; try common locations
    bool fontOK = false;
    if (font.loadFromFile("arial.ttf")) {
        fontOK = true;
    } else if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        fontOK = true;
    } else if (font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {
        fontOK = true;
    } else {
        std::cerr << "Failed to load any font; labels will not appear" << std::endl;
    }
    sf::Text evalText;
    if (fontOK) evalText.setFont(font);
    evalText.setCharacterSize(18);
    evalText.setFillColor(sf::Color::White);
    evalText.setPosition(10.f, 820.f);

    sf::RectangleShape clearBtn(sf::Vector2f(120.f, 30.f));
    clearBtn.setPosition(670.f, 820.f);
    clearBtn.setFillColor(sf::Color(150, 50, 50));

    const std::vector<char> pieceOrder = {'.','P','N','B','R','Q','K','p','n','b','r','q','k'};

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (clearBtn.getGlobalBounds().contains(x, y)) {
                        board.clear();
                    } else if (y < 800) {
                        int c = x / 100;
                        int r = y / 100;
                        char cur = board.getPieceSymbol(r, c);
                        auto it = std::find(pieceOrder.begin(), pieceOrder.end(), cur);
                        int idx = (it == pieceOrder.end() ? 0 : (it - pieceOrder.begin() + 1) % pieceOrder.size());
                        board.placePiece(pieceOrder[idx], r, c);
                    }
                }
            }
        }

        window.clear();
        drawBoard(window);

        if (board.isCheckmate(true)) {
            auto kp = board.getWhiteKingPos();
            sf::RectangleShape highlight(sf::Vector2f(100.f, 100.f));
            highlight.setPosition(kp.col * 100.f, kp.row * 100.f);
            highlight.setFillColor(sf::Color(255, 0, 0, 100));
            window.draw(highlight);
        }
        if (board.isCheckmate(false)) {
            auto kp = board.getBlackKingPos();
            sf::RectangleShape highlight(sf::Vector2f(100.f, 100.f));
            highlight.setPosition(kp.col * 100.f, kp.row * 100.f);
            highlight.setFillColor(sf::Color(255, 0, 0, 100));
            window.draw(highlight);
        }

        window.draw(clearBtn);
        sf::Text btnText("Clear Board", font, 16);
        btnText.setPosition(674.f, 824.f);
        window.draw(btnText);

        std::string evalstr = evaluatePosition(8);
        evalText.setString(evalstr);
        window.draw(evalText);

        window.display();
    }
}

void Game::drawBoard(sf::RenderWindow& window) {
    // draw squares and pieces
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

    // draw file letters along bottom and top
    if (font.getInfo().family != "") {
        for (int c = 0; c < 8; ++c) {
            sf::Text letter;
            letter.setFont(font);
            letter.setCharacterSize(14);
            letter.setFillColor(sf::Color::Black);
            letter.setString(std::string(1, 'a' + c));
            letter.setPosition(c * 100.f + 40.f, 804.f);    // slight offset
            window.draw(letter);
            // top
            letter.setPosition(c * 100.f + 40.f, 4.f);
            window.draw(letter);
        }
        // draw rank numbers along left and right
        for (int r = 0; r < 8; ++r) {
            sf::Text number;
            number.setFont(font);
            number.setCharacterSize(14);
            number.setFillColor(sf::Color::Black);
            number.setString(std::to_string(8 - r));
            number.setPosition(4.f, r * 100.f + 35.f);
            window.draw(number);
            number.setPosition(796.f, r * 100.f + 35.f);
            window.draw(number);
        }
    }
}

std::string Game::movesToString(const std::vector<Move>& seq, bool startWhite) const {
    chessboard temp = board;
    auto coord = [&](int r, int c){
        char file = 'a' + c;
        char rank = '0' + (8 - r);
        std::string s;
        s.push_back(file);
        s.push_back(rank);
        return s;
    };
    std::ostringstream oss;
    bool turn = startWhite;
    for (const Move &m : seq) {
        char piece = temp.getPieceSymbol(m.fromRow, m.fromCol);
        if (std::tolower(piece) != 'p' && piece != '.') oss << (char)std::toupper(piece);
        oss << coord(m.fromRow, m.fromCol);
        oss << (m.capture ? "x" : "-");
        oss << coord(m.toRow, m.toCol);
        if (m.promotion && std::tolower(m.promotion) != 'q') {
            oss << ":" << std::tolower(m.promotion);
        }
        oss << " ";
        // make the move on temp so future moves have correct state
        temp.makeMove(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion);
        turn = !turn;
    }
    return oss.str();
}



std::string Game::evaluatePosition(int searchDepth) {
    std::ostringstream out;
    bool wCheck = board.isCheck(true);
    bool bCheck = board.isCheck(false);
    bool wMate = board.isCheckmate(true);
    bool bMate = board.isCheckmate(false);

    if (wMate) {
        out << "White is checkmated!";
        return out.str();
    }
    if (bMate) {
        out << "Black is checkmated!";
        return out.str();
    }

    if (wCheck) out << "White is in check.";
    if (bCheck) {
        if (wCheck) out << " ";
        out << "Black is in check.";
    }
    if (wCheck || bCheck) out << "\n";

    bool whiteToMove = true;
    if (bCheck && !wCheck) whiteToMove = false;
    else if (wCheck && !bCheck) whiteToMove = true;

    std::vector<Move> seq;
    if (whiteToMove) {
        int depth = board.findMate(searchDepth, true, seq);
        if (depth >= 0) {
            out << "White mates in " << depth << " (depth " << searchDepth << "): ";
            out << movesToString(seq, true);
            return out.str();
        }
        depth = board.findMate(searchDepth, false, seq);
        if (depth >= 0) {
            out << "Black mates in " << depth << " (depth " << searchDepth << "): ";
            out << movesToString(seq, false);
            return out.str();
        }
    } else {
        int depth = board.findMate(searchDepth, false, seq);
        if (depth >= 0) {
            out << "Black mates in " << depth << " (depth " << searchDepth << "): ";
            out << movesToString(seq, false);
            return out.str();
        }
        depth = board.findMate(searchDepth, true, seq);
        if (depth >= 0) {
            out << "White mates in " << depth << " (depth " << searchDepth << "): ";
            out << movesToString(seq, true);
            return out.str();
        }
    }
    out << "No forced mate found (depth " << searchDepth << ").";
    return out.str();
}

