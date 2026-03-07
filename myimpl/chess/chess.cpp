#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
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

    const auto& targetPiece = getElement(toRow, toCol);
    if (targetPiece && std::tolower(targetPiece->getSymbol()) == 'k') {
        return false; // do not capture kings
    }

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

bool chessboard::makeMoveUndo(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece, MoveRecord& rec) {
    if (isempty(fromRow, fromCol)) return false;
    rec.fromR = fromRow;
    rec.fromC = fromCol;
    rec.toR = toRow;
    rec.toC = toCol;
    rec.prevWhiteKing = whiteKingPos;
    rec.prevBlackKing = blackKingPos;

    PiecePtr& movingPiece = getElement(fromRow, fromCol);
    bool isWhite = movingPiece->isWhite();
    char symbol = movingPiece->getSymbol();
    rec.originalType = symbol;
    rec.moved = movingPiece->hasMoved();
    rec.captured = nullptr;

    if (!movingPiece->isvalidmove(toRow, toCol, *this)) return false;

    chessboard tmp(*this);
    if (!tmp.makeMove(fromRow, fromCol, toRow, toCol, promotionPiece)) return false;

    if (!isempty(toRow, toCol)) {
        rec.captured = std::move(getElement(toRow, toCol));
    }

    if (std::tolower(symbol) == 'k' && std::abs(toCol - fromCol) == 2) {
        rec.castling = true;
        rec.rookFromC = (toCol > fromCol) ? 7 : 0;
        rec.rookToC = (toCol > fromCol) ? 5 : 3;
        PiecePtr& rook = getElement(toRow, rec.rookFromC);
        rec.rookMoved = rook->hasMoved();
    } else {
        rec.castling = false;
    }

    setElement(toRow, toCol, std::move(getElement(fromRow, fromCol)));
    setElement(fromRow, fromCol, nullptr);
    PiecePtr& p = getElement(toRow, toCol);
    p->setRow(toRow);
    p->setCol(toCol);
    p->setMoved(true);

    if (std::tolower(symbol) == 'k') {
        if (isWhite) whiteKingPos = {toRow, toCol};
        else blackKingPos = {toRow, toCol};
    }

    if ((symbol == 'P' && toRow == 0) || (symbol == 'p' && toRow == 7)) {
        rec.promotion = true;
        bool w = p->isWhite();
        char choice = std::tolower(promotionPiece);
        if (choice == 'r') setElement(toRow, toCol, std::make_unique<rook>(toRow, toCol, w));
        else if (choice == 'n') setElement(toRow, toCol, std::make_unique<knight>(toRow, toCol, w));
        else if (choice == 'b') setElement(toRow, toCol, std::make_unique<bishop>(toRow, toCol, w));
        else setElement(toRow, toCol, std::make_unique<queen>(toRow, toCol, w));
        getElement(toRow, toCol)->setMoved(true);
    } else {
        rec.promotion = false;
    }

    if (rec.castling) {
        PiecePtr& rook = getElement(toRow, rec.rookFromC);
        setElement(toRow, rec.rookToC, std::move(rook));
        setElement(toRow, rec.rookFromC, nullptr);
        getElement(toRow, rec.rookToC)->setCol(rec.rookToC);
        getElement(toRow, rec.rookToC)->setMoved(true);
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
        auto& p = getElement(rec.fromR, rec.fromC);
        p->setMoved(rec.moved);
    } else {
        setElement(rec.fromR, rec.fromC, std::move(getElement(rec.toR, rec.toC)));
        auto& p = getElement(rec.fromR, rec.fromC);
        p->setRow(rec.fromR);
        p->setCol(rec.fromC);
        p->setMoved(rec.moved);
    }

    if (rec.captured) {
        setElement(rec.toR, rec.toC, std::move(const_cast<PiecePtr&>(rec.captured)));
    } else {
        setElement(rec.toR, rec.toC, nullptr);
    }
}

std::vector<Move> chessboard::generateLegalMoves(bool whiteTurn, bool sortCaptures) const {
    std::vector<Move> moves;
    for (int r1 = 0; r1 < BOARD_SIZE; ++r1) {
        for (int c1 = 0; c1 < BOARD_SIZE; ++c1) {
            const auto& p = getElement(r1, c1);
            if (!p || p->isWhite() != whiteTurn) continue;
            for (int r2 = 0; r2 < BOARD_SIZE; ++r2) {
                for (int c2 = 0; c2 < BOARD_SIZE; ++c2) {
                    MoveRecord rec;
                    chessboard tmp(*this);
                    if (tmp.makeMove(r1, c1, r2, c2, 'q')) {
                        Move m{r1,c1,r2,c2,'q'};
                        m.capture = !isempty(r2, c2);
                        moves.push_back(m);
                    }
                }
            }
        }
    }
    if (sortCaptures) {
        std::sort(moves.begin(), moves.end(), [&](const Move&a,const Move&b){
            return a.capture > b.capture;
        });
    }
    return moves;
}

int chessboard::findMate(int maxDepth, bool whiteToMove, std::vector<Move>& sequence) const {
        chessboard b(*this);
    std::function<int(chessboard&, bool,bool,int,std::vector<Move>&)> rec;
    rec = [&](chessboard &board, bool attacker, bool turn, int depth, std::vector<Move>& seq) -> int {
        if (board.isCheckmate(turn)) {
            if (turn != attacker) return 0;
            return -1;
        }
        if (board.isCheckmate(!turn)) {
            if (turn == attacker) return 0;
            return -1;
        }
        if (depth == 0) return -1;

        if (turn == attacker) {
            std::vector<Move> moves = board.generateLegalMoves(turn, true);
            for (const Move &m : moves) {
                MoveRecord recd;
                if (!board.makeMoveUndo(m.fromRow,m.fromCol,m.toRow,m.toCol,m.promotion,recd)) continue;
                std::vector<Move> childSeq;
                int res = rec(board, attacker, !turn, depth-1, childSeq);
                board.undoMove(recd);
                if (res >= 0) {
                    seq.clear();
                    seq.push_back(m);
                    seq.insert(seq.end(), childSeq.begin(), childSeq.end());
                    return res + 1;
                }
            }
            return -1;
        } else {
            std::vector<Move> moves = board.generateLegalMoves(turn, true);
            if (moves.empty()) return -1;
            int best = -1;
            std::vector<Move> bestSeq;
            for (const Move &m : moves) {
                MoveRecord recd;
                if (!board.makeMoveUndo(m.fromRow,m.fromCol,m.toRow,m.toCol,m.promotion,recd)) continue;
                std::vector<Move> childSeq;
                int res = rec(board, attacker, !turn, depth-1, childSeq);
                board.undoMove(recd);
                if (res == -1) {
                    return -1;
                }
                if (best == -1 || res < best) {
                    best = res;
                    bestSeq.clear();
                    bestSeq.push_back(m);
                    bestSeq.insert(bestSeq.end(), childSeq.begin(), childSeq.end());
                }
            }
            if (best == -1) return -1;
            seq = bestSeq;
            return best + 1;
        }
    };
    return rec(b, whiteToMove, whiteToMove, maxDepth, sequence);
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

bool chessboard::isCheckmate(bool whiteTurn) const {
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

bool chessboard::isStalemate(bool whiteTurn) const {
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

position chessboard::getWhiteKingPos() const {
    return whiteKingPos;
}

position chessboard::getBlackKingPos() const {
    return blackKingPos;
}

void chessboard::clear() {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            this->setElement(i, j, nullptr);
    whiteKingPos = {-1,-1};
    blackKingPos = {-1,-1};
}

void chessboard::placePiece(char symbol, int row, int col) {
    this->setElement(row, col, nullptr);
    if (symbol == '.' || symbol == '\0') return;
    bool isWhite = std::isupper(symbol);
    char lower = std::tolower(symbol);
    PiecePtr p;
    switch (lower) {
        case 'p': p = std::make_unique<pawn>(row, col, isWhite); break;
        case 'r': p = std::make_unique<rook>(row, col, isWhite); break;
        case 'n': p = std::make_unique<knight>(row, col, isWhite); break;
        case 'b': p = std::make_unique<bishop>(row, col, isWhite); break;
        case 'q': p = std::make_unique<queen>(row, col, isWhite); break;
        case 'k': p = std::make_unique<king>(row, col, isWhite); 
                  if (isWhite) whiteKingPos = {row, col};
                  else blackKingPos = {row, col};
                  break;
        default: return;
    }
    if (p) setElement(row, col, std::move(p));
}

static bool isKingCheck(const chessboard &board, bool whiteTurn) {
    return board.isCheck(whiteTurn);
}

