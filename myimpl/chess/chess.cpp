#include <iostream>
#include <cmath>
#include <algorithm>
#include "chess.h"

chessboard::chessboard() : Matrix<PiecePtr>(BOARD_SIZE) {
    initChessboard();
}

chessboard::chessboard(const chessboard& other)
    : Matrix<PiecePtr>(other.getSize()),
      whiteKingPos(other.whiteKingPos),
      blackKingPos(other.blackKingPos)
{
    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < m_size; ++j) {
            const auto& p = other.getElement(i, j);
            this->setElement(i, j, p ? p->clone() : nullptr);
        }
    }
}

chessboard& chessboard::operator=(const chessboard& other) {
    if (this != &other) {
        this->whiteKingPos = other.whiteKingPos;
        this->blackKingPos = other.blackKingPos;

        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                const auto& p = other.getElement(i, j);
                this->setElement(i, j, p ? p->clone() : nullptr);
            }
        }
    }
    return *this;
}

chessboard::~chessboard() {}

bool chessboard::makeMove(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece) {
    if (isempty(fromRow, fromCol)) return false;

    PiecePtr& movingPiece = getElement(fromRow, fromCol);
    bool isWhite = movingPiece->isWhite();
    char symbol = movingPiece->getSymbol();

    if (!movingPiece->isvalidmove(toRow, toCol, *this)) return false;

    chessboard tempBoard(*this);
    PiecePtr& tempP = tempBoard.getElement(fromRow, fromCol);
    
    if (std::tolower(symbol) == 'k') {
        if (isWhite) tempBoard.whiteKingPos = {toRow, toCol};
        else tempBoard.blackKingPos = {toRow, toCol};
    }

    tempBoard.setElement(toRow, toCol, std::move(tempP));
    tempBoard.setElement(fromRow, fromCol, nullptr);

    if (tempBoard.isCheck(isWhite)) return false;

    //(Castling)
    if (std::tolower(symbol) == 'k' && std::abs(toCol - fromCol) == 2) {
        int rookFromCol = (toCol > fromCol) ? 7 : 0;
        int rookToCol = (toCol > fromCol) ? 5 : 3;
        
        PiecePtr& rook = getElement(toRow, rookFromCol);
        setElement(toRow, rookToCol, std::move(rook));
        setElement(toRow, rookFromCol, nullptr);
        
        getElement(toRow, rookToCol)->setCol(rookToCol);
        getElement(toRow, rookToCol)->setMoved(true);
    }

    if (symbol == 'K') whiteKingPos = {toRow, toCol};
    else if (symbol == 'k') blackKingPos = {toRow, toCol};

    setElement(toRow, toCol, std::move(movingPiece));
    setElement(fromRow, fromCol, nullptr);

    auto& p = getElement(toRow, toCol);
    p->setRow(toRow);
    p->setCol(toCol);
    p->setMoved(true);

    //(Promotion)
    if ((symbol == 'P' && toRow == 0) || (symbol == 'p' && toRow == 7)) {
        char choice = std::tolower(promotionPiece);
        if (choice == 'r') setElement(toRow, toCol, std::make_unique<rook>(toRow, toCol, isWhite));
        else if (choice == 'n') setElement(toRow, toCol, std::make_unique<knight>(toRow, toCol, isWhite));
        else if (choice == 'b') setElement(toRow, toCol, std::make_unique<bishop>(toRow, toCol, isWhite));
        else setElement(toRow, toCol, std::make_unique<queen>(toRow, toCol, isWhite));
        
        getElement(toRow, toCol)->setMoved(true);
    }

    return true;
}

void chessboard::initChessboard() {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            this->setElement(i, j, nullptr);

    this->setElement(0, 0, std::make_unique<rook>(0, 0, false));
    this->setElement(0, 1, std::make_unique<knight>(0, 1, false));
    this->setElement(0, 2, std::make_unique<bishop>(0, 2, false));
    this->setElement(0, 3, std::make_unique<queen>(0, 3, false));
    this->setElement(0, 4, std::make_unique<king>(0, 4, false));
    blackKingPos = {0, 4};
    this->setElement(0, 5, std::make_unique<bishop>(0, 5, false));
    this->setElement(0, 6, std::make_unique<knight>(0, 6, false));
    this->setElement(0, 7, std::make_unique<rook>(0, 7, false));
    for (int j = 0; j < BOARD_SIZE; ++j) this->setElement(1, j, std::make_unique<pawn>(1, j, false));

    this->setElement(7, 0, std::make_unique<rook>(7, 0, true));
    this->setElement(7, 1, std::make_unique<knight>(7, 1, true));
    this->setElement(7, 2, std::make_unique<bishop>(7, 2, true));
    this->setElement(7, 3, std::make_unique<queen>(7, 3, true));
    this->setElement(7, 4, std::make_unique<king>(7, 4, true));
    whiteKingPos = {7, 4};
    this->setElement(7, 5, std::make_unique<bishop>(7, 5, true));
    this->setElement(7, 6, std::make_unique<knight>(7, 6, true));
    this->setElement(7, 7, std::make_unique<rook>(7, 7, true));
    for (int j = 0; j < BOARD_SIZE; ++j) this->setElement(6, j, std::make_unique<pawn>(6, j, true));
}

bool chessboard::isCheck(bool whiteKing) const {
    position kingPos = whiteKing ? whiteKingPos : blackKingPos;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            const auto& p = getElement(i, j);
            if (p && p->isWhite() != whiteKing) {
                if (p->isvalidmove(kingPos.row, kingPos.col, *this)) return true;
            }
        }
    }
    return false;
}

bool chessboard::isCheckmate(bool whiteTurn) {
    if (!isCheck(whiteTurn)) return false;

    for (int r1 = 0; r1 < BOARD_SIZE; ++r1) {
        for (int c1 = 0; c1 < BOARD_SIZE; ++c1) {
            const auto& p = getElement(r1, c1);
            if (p && p->isWhite() == whiteTurn) {
                for (int r2 = 0; r2 < BOARD_SIZE; ++r2) {
                    for (int c2 = 0; c2 < BOARD_SIZE; ++c2) {
                        chessboard tempBoard(*this);
                        if (tempBoard.makeMove(r1, c1, r2, c2, 'q')) return false; 
                    }
                }
            }
        }
    }
    return true;
}

bool chessboard::isStalemate(bool whiteTurn) {
    if (isCheck(whiteTurn)) return false;

    for (int r1 = 0; r1 < BOARD_SIZE; ++r1) {
        for (int c1 = 0; c1 < BOARD_SIZE; ++c1) {
            const auto& p = getElement(r1, c1);
            if (p && p->isWhite() == whiteTurn) {
                for (int r2 = 0; r2 < BOARD_SIZE; ++r2) {
                    for (int c2 = 0; c2 < BOARD_SIZE; ++c2) {
                        chessboard tempBoard(*this);
                        if (tempBoard.makeMove(r1, c1, r2, c2, 'q')) return false;
                    }
                }
            }
        }
    }
    return true;
}

void chessboard::printChessboard() const {
    std::cout << "\n    a b c d e f g h\n  +-----------------+\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << 8 - i << " | "; 
        for (int j = 0; j < BOARD_SIZE; ++j) {
            std::cout << getPieceSymbol(i, j) << " ";
        }
        std::cout << "| " << 8 - i << std::endl;
    }
    std::cout << "  +-----------------+\n    a b c d e f g h\n\n";
}

char chessboard::getPieceSymbol(int row, int col) const {
    const auto& p = this->getElement(row, col);
    return p ? p->getSymbol() : '.';
}

bool chessboard::isempty(int row, int col) const {
    return this->getElement(row, col) == nullptr;
}