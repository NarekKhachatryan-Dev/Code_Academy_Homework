#ifndef CHESS_H
#define CHESS_H

#include <memory>
#include <vector>
#include "matrix.h"

class piece;
using PiecePtr = std::unique_ptr<piece>;

class piece {
public:
    int m_row;
    int m_col;
    char m_type;
    bool m_isWhite;
    bool m_hasMoved = false;

    piece(int row, int col, char type, bool isWhite);
    piece(const piece& other);
    virtual ~piece() = default;

    virtual PiecePtr clone() const = 0;
    virtual bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const;
    void setRow(int row) { m_row = row; }
    void setCol(int col) { m_col = col; }
    bool hasMoved() const { return m_hasMoved; }
    void setMoved(bool moved) { m_hasMoved = moved; }

    char getSymbol() const;
    bool isWhite() const;
};

#define CHESS_PIECE_CLASS(ClassName) \
class ClassName : public piece { \
public: \
    ClassName(int row, int col, bool isWhite); \
    PiecePtr clone() const override { return std::make_unique<ClassName>(*this); } \
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override; \
    ~ClassName() override = default; \
};

CHESS_PIECE_CLASS(pawn)
CHESS_PIECE_CLASS(bishop)
CHESS_PIECE_CLASS(knight)
CHESS_PIECE_CLASS(rook)
CHESS_PIECE_CLASS(queen)
CHESS_PIECE_CLASS(king)

struct position {
    int row;
    int col;
};


struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    char promotion = 'q';
    bool capture = false;
};

class chessboard : public Matrix<PiecePtr> {
public:
    static constexpr int BOARD_SIZE = 8;
    position whiteKingPos;
    position blackKingPos;

    chessboard();
    chessboard(const chessboard& other);
    chessboard& operator=(const chessboard& other);
    chessboard(chessboard&& other) noexcept = default;
    chessboard& operator=(chessboard&& other) noexcept = default;

    void initChessboard();
    void clear();
    void printChessboard() const;

    bool isCheckmate(bool whiteTurn) const;
    bool isStalemate(bool whiteTurn) const;
    bool isCheck(bool whiteTurn) const;

    char getPieceSymbol(int row, int col) const;
    bool isempty(int row, int col) const;
    void placePiece(char symbol, int row, int col);

    bool makeMove(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece = 'q');

    struct MoveRecord {
        int fromR, fromC, toR, toC;
        PiecePtr captured;
        bool moved;
        bool castling = false;
        int rookFromC, rookToC;
        bool rookMoved;
        bool promotion = false;
        char originalType;
        position prevWhiteKing;
        position prevBlackKing;
    };

    bool makeMoveUndo(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece, MoveRecord& rec);
    void undoMove(const MoveRecord& rec);

    std::vector<Move> generateLegalMoves(bool whiteTurn, bool sortCaptures=false) const;

    int findMate(int maxDepth, bool whiteToMove, std::vector<Move>& sequence) const;

    position getWhiteKingPos() const;
    position getBlackKingPos() const;

    virtual ~chessboard();
};

#endif