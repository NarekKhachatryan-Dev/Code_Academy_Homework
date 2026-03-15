#ifndef CHESS_H
#define CHESS_H

#include <memory>
#include <vector>
#include <string>
#include "matrix.h"

class piece;
using PiecePtr = std::unique_ptr<piece>;

struct position {
    int row;
    int col;
};

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
    virtual bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const = 0;
    
    void setRow(int row) { m_row = row; }
    void setCol(int col) { m_col = col; }
    bool hasMoved() const { return m_hasMoved; }
    void setMoved(bool moved) { m_hasMoved = moved; }
    char getSymbol() const { return m_type; }
    bool isWhite() const { return m_isWhite; }
};

class pawn : public piece {
public:
    pawn(int row, int col, bool isWhite);
    PiecePtr clone() const override { return std::make_unique<pawn>(*this); }
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override;
    ~pawn() override = default;
};

class bishop : public piece {
public:
    bishop(int row, int col, bool isWhite);
    PiecePtr clone() const override { return std::make_unique<bishop>(*this); }
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override;
    ~bishop() override = default;
};

class knight : public piece {
public:
    knight(int row, int col, bool isWhite);
    PiecePtr clone() const override { return std::make_unique<knight>(*this); }
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override;
    ~knight() override = default;
};

class rook : public piece {
public:
    rook(int row, int col, bool isWhite);
    PiecePtr clone() const override { return std::make_unique<rook>(*this); }
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override;
    ~rook() override = default;
};

class queen : public piece {
public:
    queen(int row, int col, bool isWhite);
    PiecePtr clone() const override { return std::make_unique<queen>(*this); }
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override;
    ~queen() override = default;
};

class king : public piece {
public:
    king(int row, int col, bool isWhite);
    PiecePtr clone() const override { return std::make_unique<king>(*this); }
    bool isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const override;
    ~king() override = default;
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

    chessboard();
    chessboard(const chessboard& other);
    chessboard& operator=(const chessboard& other);
    virtual ~chessboard();

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
    bool makeMoveUndo(int fromRow, int fromCol, int toRow, int toCol, char promotionPiece, MoveRecord& rec);
    void undoMove(MoveRecord& rec);
    int findMate(int maxDepth, bool whiteToMove, std::vector<Move>& sequence);
    
    std::vector<Move> generateLegalMoves(bool whiteTurn, bool sortCaptures = false);
    int evaluate() const;
    int analyze(int depth, int alpha, int beta, bool maximizingPlayer);

    position getWhiteKingPos() const { return whiteKingPos; }
    position getBlackKingPos() const { return blackKingPos; }

private:
    static PiecePtr createPieceBySymbol(char symbol, int row, int col);
    void refreshKingPositions();
};

#endif