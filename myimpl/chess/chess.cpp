#include <iostream>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <functional>
#include <vector>
#include "chess.h"

namespace {
int pieceValue(char symbol) {
    switch (std::tolower(symbol)) {
        case 'p': return 100;
        case 'n': return 320;
        case 'b': return 330;
        case 'r': return 500;
        case 'q': return 900;
        case 'k': return 20000;
        default: return 0;
    }
}
}

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

PiecePtr chessboard::createPieceBySymbol(char symbol, int row, int col) {
    bool isWhite = std::isupper(symbol);
    switch (std::tolower(symbol)) {
        case 'p': return std::make_unique<pawn>(row, col, isWhite);
        case 'r': return std::make_unique<rook>(row, col, isWhite);
        case 'n': return std::make_unique<knight>(row, col, isWhite);
        case 'b': return std::make_unique<bishop>(row, col, isWhite);
        case 'q': return std::make_unique<queen>(row, col, isWhite);
        case 'k': return std::make_unique<king>(row, col, isWhite);
        default: return nullptr;
    }
}

void chessboard::refreshKingPositions() {
    position newWhite{-1, -1};
    position newBlack{-1, -1};

    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            const auto& p = getElement(r, c);
            if (!p || std::tolower(p->getSymbol()) != 'k') continue;
            if (p->isWhite()) {
                if (newWhite.row == -1) newWhite = {r, c};
            } else {
                if (newBlack.row == -1) newBlack = {r, c};
            }
        }
    }

    whiteKingPos = newWhite;
    blackKingPos = newBlack;
}

bool chessboard::makeMove(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece) {
    MoveRecord rec;
    return makeMoveUndo(fromRow, fromCol, toRow, toCol, promotionPiece, rec);
}

bool chessboard::makeMoveUndo(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece, MoveRecord& rec) {
    if (isempty(fromRow, fromCol)) return false;

    PiecePtr& movingPiece = getElement(fromRow, fromCol);
    bool isWhite = movingPiece->isWhite();
    
    
    if (!movingPiece->isvalidmove(toRow, toCol, *this)) return false;

    
    rec.fromR = fromRow; rec.fromC = fromCol;
    rec.toR = toRow; rec.toC = toCol;
    rec.prevWhiteKing = whiteKingPos;
    rec.prevBlackKing = blackKingPos;
    rec.originalType = movingPiece->getSymbol();
    rec.moved = movingPiece->hasMoved();
    rec.captured = std::move(getElement(toRow, toCol));
    rec.promotion = false;
    rec.castling = false;

    char symbol = std::tolower(rec.originalType);

    
    if (symbol == 'k' && std::abs(toCol - fromCol) == 2) {
        rec.castling = true;
        rec.rookFromC = (toCol > fromCol) ? 7 : 0;
        rec.rookToC = (toCol > fromCol) ? 5 : 3;
    }

    
    setElement(toRow, toCol, std::move(movingPiece));
    setElement(fromRow, fromCol, nullptr);

    PiecePtr& p = getElement(toRow, toCol);
    p->setRow(toRow);
    p->setCol(toCol);
    p->setMoved(true);

    if (symbol == 'k') {
        if (isWhite) whiteKingPos = {toRow, toCol};
        else blackKingPos = {toRow, toCol};
    }

    
    if (isCheck(isWhite)) {
        undoMove(rec);
        return false;
    }

    
    if (rec.castling) {
        PiecePtr& rook = getElement(toRow, rec.rookFromC);
        rec.rookMoved = rook->hasMoved();
        setElement(toRow, rec.rookToC, std::move(rook));
        setElement(toRow, rec.rookFromC, nullptr);
        getElement(toRow, rec.rookToC)->setCol(rec.rookToC);
        getElement(toRow, rec.rookToC)->setMoved(true);
    }

    
    if (symbol == 'p' && (toRow == 0 || toRow == 7)) {
        rec.promotion = true;
        char choice = std::tolower(promotionPiece);
        if (choice == 'r') setElement(toRow, toCol, createPieceBySymbol(isWhite ? 'R' : 'r', toRow, toCol));
        else if (choice == 'n') setElement(toRow, toCol, createPieceBySymbol(isWhite ? 'N' : 'n', toRow, toCol));
        else if (choice == 'b') setElement(toRow, toCol, createPieceBySymbol(isWhite ? 'B' : 'b', toRow, toCol));
        else setElement(toRow, toCol, createPieceBySymbol(isWhite ? 'Q' : 'q', toRow, toCol));
        getElement(toRow, toCol)->setMoved(true);
    }

    return true;
}

void chessboard::undoMove(MoveRecord& rec) {
    whiteKingPos = rec.prevWhiteKing;
    blackKingPos = rec.prevBlackKing;

    if (rec.castling) {
        PiecePtr& rook = getElement(rec.toR, rec.rookToC);
        setElement(rec.toR, rec.rookFromC, std::move(rook));
        setElement(rec.toR, rec.rookToC, nullptr);
        getElement(rec.toR, rec.rookFromC)->setCol(rec.rookFromC);
        getElement(rec.toR, rec.rookFromC)->setMoved(rec.rookMoved);
    }

    if (rec.promotion) {
        bool w = std::isupper(rec.originalType);
        setElement(rec.fromR, rec.fromC, createPieceBySymbol(w ? 'P' : 'p', rec.fromR, rec.fromC));
        getElement(rec.fromR, rec.fromC)->setMoved(rec.moved);
    } else {
        setElement(rec.fromR, rec.fromC, std::move(getElement(rec.toR, rec.toC)));
        auto& p = getElement(rec.fromR, rec.fromC);
        p->setRow(rec.fromR);
        p->setCol(rec.fromC);
        p->setMoved(rec.moved);
    }

    setElement(rec.toR, rec.toC, std::move(rec.captured));
}

void chessboard::placePiece(char symbol, int row, int col) {
    if (symbol == '.') {
        setElement(row, col, nullptr);
        this->refreshKingPositions();
        return;
    }

    PiecePtr created = createPieceBySymbol(symbol, row, col);
    if (!created) return;
    setElement(row, col, std::move(created));

    this->refreshKingPositions();
}

int chessboard::findMate(int maxDepth, bool whiteToMove, std::vector<Move>& sequence) {
    if (maxDepth <= 0) return -1;

    const bool attackerIsWhite = whiteToMove;
    constexpr int INF = 1000000;

    std::function<int(int, bool, std::vector<Move>&)> dfs = [&](int depth, bool sideToMove, std::vector<Move>& line) -> int {
        if (isCheckmate(sideToMove)) {
            line.clear();
            return (sideToMove != attackerIsWhite) ? 0 : -1;
        }

        if (depth == 0) return -1;

        std::vector<Move> moves = generateLegalMoves(sideToMove, true);
        if (moves.empty()) return -1;

        if (sideToMove == attackerIsWhite) {
            int best = INF;
            std::vector<Move> bestLine;

            for (const auto& m : moves) {
                MoveRecord rec;
                if (!makeMoveUndo(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion, rec)) continue;

                std::vector<Move> childLine;
                int res = dfs(depth - 1, !sideToMove, childLine);
                undoMove(rec);

                if (res == -1) continue;
                int mateDepth = res + 1;
                if (mateDepth < best) {
                    best = mateDepth;
                    bestLine.clear();
                    bestLine.push_back(m);
                    bestLine.insert(bestLine.end(), childLine.begin(), childLine.end());
                }
            }

            if (best == INF) return -1;
            line = bestLine;
            return best;
        }

        int worst = -1;
        std::vector<Move> worstLine;

        for (const auto& m : moves) {
            MoveRecord rec;
            if (!makeMoveUndo(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion, rec)) continue;

            std::vector<Move> childLine;
            int res = dfs(depth - 1, !sideToMove, childLine);
            undoMove(rec);

            if (res == -1) return -1;

            int mateDepth = res + 1;
            if (mateDepth > worst) {
                worst = mateDepth;
                worstLine.clear();
                worstLine.push_back(m);
                worstLine.insert(worstLine.end(), childLine.begin(), childLine.end());
            }
        }

        if (worst == -1) return -1;
        line = worstLine;
        return worst;
    };

    return dfs(maxDepth, whiteToMove, sequence);
}

std::vector<Move> chessboard::generateLegalMoves(bool whiteTurn, bool sortCaptures) {
    std::vector<Move> moves;
    for (int r1 = 0; r1 < BOARD_SIZE; ++r1) {
        for (int c1 = 0; c1 < BOARD_SIZE; ++c1) {
            auto& p = getElement(r1, c1);
            if (!p || p->isWhite() != whiteTurn) continue;

            for (int r2 = 0; r2 < BOARD_SIZE; ++r2) {
                for (int c2 = 0; c2 < BOARD_SIZE; ++c2) {
                    MoveRecord rec;
                    
                    if (this->makeMoveUndo(r1, c1, r2, c2, 'q', rec)) {
                        Move m{r1, c1, r2, c2, 'q'};
                        m.capture = (rec.captured != nullptr);
                        moves.push_back(m);
                        this->undoMove(rec);
                    }
                }
            }
        }
    }
    if (sortCaptures) {
        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            return a.capture > b.capture;
        });
    }
    return moves;
}

int chessboard::evaluate() const {
    int score = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            const auto& p = getElement(i, j);
            if (p) {
                int val = pieceValue(p->getSymbol());
                score += p->isWhite() ? val : -val;
            }
        }
    }
    return score;
}

int chessboard::analyze(int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) return evaluate();

    std::vector<Move> moves = generateLegalMoves(maximizingPlayer, true);
    if (moves.empty()) {
        if (isCheck(maximizingPlayer)) return maximizingPlayer ? -100000 : 100000;
        return 0;
    }

    if (maximizingPlayer) {
        int maxEval = -1000000;
        for (const auto& m : moves) {
            MoveRecord rec;
            if (makeMoveUndo(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion, rec)) {
                int eval = analyze(depth - 1, alpha, beta, false);
                undoMove(rec);
                maxEval = std::max(maxEval, eval);
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break;
            }
        }
        return maxEval;
    } else {
        int minEval = 1000000;
        for (const auto& m : moves) {
            MoveRecord rec;
            if (makeMoveUndo(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion, rec)) {
                int eval = analyze(depth - 1, alpha, beta, true);
                undoMove(rec);
                minEval = std::min(minEval, eval);
                beta = std::min(beta, eval);
                if (beta <= alpha) break;
            }
        }
        return minEval;
    }
}

void chessboard::initChessboard() {
    clear();
    setElement(0, 0, createPieceBySymbol('r', 0, 0));
    setElement(0, 1, createPieceBySymbol('n', 0, 1));
    setElement(0, 2, createPieceBySymbol('b', 0, 2));
    setElement(0, 3, createPieceBySymbol('q', 0, 3));
    setElement(0, 4, createPieceBySymbol('k', 0, 4));
    blackKingPos = {0, 4};
    setElement(0, 5, createPieceBySymbol('b', 0, 5));
    setElement(0, 6, createPieceBySymbol('n', 0, 6));
    setElement(0, 7, createPieceBySymbol('r', 0, 7));
    for (int j = 0; j < BOARD_SIZE; ++j) setElement(1, j, createPieceBySymbol('p', 1, j));

    setElement(7, 0, createPieceBySymbol('R', 7, 0));
    setElement(7, 1, createPieceBySymbol('N', 7, 1));
    setElement(7, 2, createPieceBySymbol('B', 7, 2));
    setElement(7, 3, createPieceBySymbol('Q', 7, 3));
    setElement(7, 4, createPieceBySymbol('K', 7, 4));
    whiteKingPos = {7, 4};
    setElement(7, 5, createPieceBySymbol('B', 7, 5));
    setElement(7, 6, createPieceBySymbol('N', 7, 6));
    setElement(7, 7, createPieceBySymbol('R', 7, 7));
    for (int j = 0; j < BOARD_SIZE; ++j) setElement(6, j, createPieceBySymbol('P', 6, j));
}

bool chessboard::isCheck(bool whiteKing) const {
    
    position kingPos = whiteKing ? whiteKingPos : blackKingPos;
    
    
    if (kingPos.row == -1 || kingPos.col == -1) return false;

    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            const auto& p = getElement(i, j);
            
            
            if (p && p->isWhite() != whiteKing) {
                
                if (p->isvalidmove(kingPos.row, kingPos.col, *this)) {
                    return true; 
                }
            }
        }
    }
    return false;
}

bool chessboard::isCheckmate(bool whiteTurn) const {
    
    if (!isCheck(whiteTurn)) return false;

    
    chessboard temp = *this;
    auto moves = temp.generateLegalMoves(whiteTurn);
    
    return moves.empty();
}

bool chessboard::isStalemate(bool whiteTurn) const {
    
    if (isCheck(whiteTurn)) return false;

    
    chessboard temp(*this);
    auto moves = temp.generateLegalMoves(whiteTurn);
    
    return moves.empty(); 
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

void chessboard::clear() {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            this->setElement(i, j, nullptr);
    whiteKingPos = {-1,-1};
    blackKingPos = {-1,-1};
}