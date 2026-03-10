#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <vector>
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
        if (choice == 'r') setElement(toRow, toCol, std::make_unique<rook>(toRow, toCol, isWhite));
        else if (choice == 'n') setElement(toRow, toCol, std::make_unique<knight>(toRow, toCol, isWhite));
        else if (choice == 'b') setElement(toRow, toCol, std::make_unique<bishop>(toRow, toCol, isWhite));
        else setElement(toRow, toCol, std::make_unique<queen>(toRow, toCol, isWhite));
        getElement(toRow, toCol)->setMoved(true);
    }

    return true;
}

void chessboard::undoMove(const MoveRecord& rec) {
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
        setElement(rec.fromR, rec.fromC, std::make_unique<pawn>(rec.fromR, rec.fromC, w));
        getElement(rec.fromR, rec.fromC)->setMoved(rec.moved);
    } else {
        setElement(rec.fromR, rec.fromC, std::move(getElement(rec.toR, rec.toC)));
        auto& p = getElement(rec.fromR, rec.fromC);
        p->setRow(rec.fromR);
        p->setCol(rec.fromC);
        p->setMoved(rec.moved);
    }

    setElement(rec.toR, rec.toC, std::move(const_cast<PiecePtr&>(rec.captured)));
}

void chessboard::placePiece(char symbol, int row, int col) {
    if (symbol == '.') {
        
        const auto& currentPiece = getElement(row, col);
        if (currentPiece && std::tolower(currentPiece->getSymbol()) == 'k') {
            if (currentPiece->isWhite()) whiteKingPos = {-1, -1};
            else blackKingPos = {-1, -1};
        }
        setElement(row, col, nullptr);
        return;
    }

    bool isWhite = std::isupper(symbol);
    char type = std::tolower(symbol);

    if (type == 'p') setElement(row, col, std::make_unique<pawn>(row, col, isWhite));
    else if (type == 'r') setElement(row, col, std::make_unique<rook>(row, col, isWhite));
    else if (type == 'n') setElement(row, col, std::make_unique<knight>(row, col, isWhite));
    else if (type == 'b') setElement(row, col, std::make_unique<bishop>(row, col, isWhite));
    else if (type == 'q') setElement(row, col, std::make_unique<queen>(row, col, isWhite));
    else if (type == 'k') {
        setElement(row, col, std::make_unique<king>(row, col, isWhite));
        
        if (isWhite) whiteKingPos = {row, col};
        else blackKingPos = {row, col};
    }
}

int chessboard::findMate(int maxDepth, bool whiteToMove, std::vector<Move>& sequence) {
    if (maxDepth <= 0) return -1;

    std::vector<Move> moves = generateLegalMoves(whiteToMove, true);
    int bestMateDepth = 1000; 
    std::vector<Move> bestSequence;

    for (const auto& m : moves) {
        MoveRecord rec;
        if (makeMoveUndo(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion, rec)) {
            
            
            if (isCheckmate(!whiteToMove)) {
                undoMove(rec);
                sequence = { m };
                return 1; 
            }

            
            if (maxDepth > 1) {
                std::vector<Move> childSeq;
                int res = findMate(maxDepth - 1, !whiteToMove, childSeq);
                
                if (res != -1) {
                    int currentMateDepth = res + 1;
                    if (currentMateDepth < bestMateDepth) {
                        bestMateDepth = currentMateDepth;
                        bestSequence = { m };
                        bestSequence.insert(bestSequence.end(), childSeq.begin(), childSeq.end());
                    }
                }
            }
            undoMove(rec);
        }
    }

    if (bestMateDepth != 1000) {
        sequence = bestSequence;
        return bestMateDepth;
    }

    return -1;
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
                int val = 0;
                char s = std::tolower(p->getSymbol());
                if (s == 'p') val = 100;
                else if (s == 'n') val = 320;
                else if (s == 'b') val = 330;
                else if (s == 'r') val = 500;
                else if (s == 'q') val = 900;
                else if (s == 'k') val = 20000;
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
    setElement(0, 0, std::make_unique<rook>(0, 0, false));
    setElement(0, 1, std::make_unique<knight>(0, 1, false));
    setElement(0, 2, std::make_unique<bishop>(0, 2, false));
    setElement(0, 3, std::make_unique<queen>(0, 3, false));
    setElement(0, 4, std::make_unique<king>(0, 4, false));
    blackKingPos = {0, 4};
    setElement(0, 5, std::make_unique<bishop>(0, 5, false));
    setElement(0, 6, std::make_unique<knight>(0, 6, false));
    setElement(0, 7, std::make_unique<rook>(0, 7, false));
    for (int j = 0; j < BOARD_SIZE; ++j) setElement(1, j, std::make_unique<pawn>(1, j, false));

    setElement(7, 0, std::make_unique<rook>(7, 0, true));
    setElement(7, 1, std::make_unique<knight>(7, 1, true));
    setElement(7, 2, std::make_unique<bishop>(7, 2, true));
    setElement(7, 3, std::make_unique<queen>(7, 3, true));
    setElement(7, 4, std::make_unique<king>(7, 4, true));
    whiteKingPos = {7, 4};
    setElement(7, 5, std::make_unique<bishop>(7, 5, true));
    setElement(7, 6, std::make_unique<knight>(7, 6, true));
    setElement(7, 7, std::make_unique<rook>(7, 7, true));
    for (int j = 0; j < BOARD_SIZE; ++j) setElement(6, j, std::make_unique<pawn>(6, j, true));
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