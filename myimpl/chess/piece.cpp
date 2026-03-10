#include "chess.h"
#include <cmath>
#include <cctype>


piece::piece(int row, int col, char type, bool isWhite)
    : m_row(row), m_col(col), m_type(type), m_isWhite(isWhite) {}

piece::piece(const piece& other)
    : m_row(other.m_row), m_col(other.m_col), m_type(other.m_type), 
      m_isWhite(other.m_isWhite), m_hasMoved(other.m_hasMoved) {}

bool piece::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (newRow < 0 || newRow >= board.getSize() || newCol < 0 || newCol >= board.getSize()) return false;
    if (newRow == m_row && newCol == m_col) return false;
    
    const auto& targetPiece = board.getElement(newRow, newCol);
    if (targetPiece && targetPiece->isWhite() == m_isWhite) return false;
    
    return true;
}


pawn::pawn(int row, int col, bool isWhite) : piece(row, col, isWhite ? 'P' : 'p', isWhite) {}
bool pawn::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (!piece::isvalidmove(newRow, newCol, board)) return false;
    
    int direction = m_isWhite ? -1 : 1; 
    int rowDiff = newRow - m_row;
    int colDiff = std::abs(newCol - m_col);
    const auto& target = board.getElement(newRow, newCol);

    if (colDiff == 0) {
        if (rowDiff == direction) return target == nullptr; 
        if (rowDiff == 2 * direction) {
            bool atStartingRow = (m_isWhite && m_row == 6) || (!m_isWhite && m_row == 1);
            return atStartingRow && target == nullptr && board.getElement(m_row + direction, m_col) == nullptr;
        }
    } else if (colDiff == 1 && rowDiff == direction) {
        return target != nullptr;
    }
    return false;
}


knight::knight(int row, int col, bool isWhite) : piece(row, col, isWhite ? 'N' : 'n', isWhite) {}
bool knight::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (!piece::isvalidmove(newRow, newCol, board)) return false;
    int rowDiff = std::abs(newRow - m_row);
    int colDiff = std::abs(newCol - m_col);
    return (rowDiff == 1 && colDiff == 2) || (rowDiff == 2 && colDiff == 1);
}


bishop::bishop(int row, int col, bool isWhite) : piece(row, col, isWhite ? 'B' : 'b', isWhite) {}
bool bishop::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (!piece::isvalidmove(newRow, newCol, board)) return false;
    if (std::abs(newRow - m_row) != std::abs(newCol - m_col)) return false;

    int rowDir = (newRow > m_row) ? 1 : -1;
    int colDir = (newCol > m_col) ? 1 : -1;

    int r = m_row + rowDir;
    int c = m_col + colDir;
    
    
    while (r != newRow && c != newCol) {
        if (board.getElement(r, c) != nullptr) return false;
        r += rowDir;
        c += colDir;
    }
    return true;
}


rook::rook(int row, int col, bool isWhite) : piece(row, col, isWhite ? 'R' : 'r', isWhite) {}
bool rook::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (!piece::isvalidmove(newRow, newCol, board)) return false;
    if (m_row != newRow && m_col != newCol) return false;

    int rowDir = (newRow == m_row) ? 0 : (newRow > m_row ? 1 : -1);
    int colDir = (newCol == m_col) ? 0 : (newCol > m_col ? 1 : -1);

    for (int r = m_row + rowDir, c = m_col + colDir; r != newRow || c != newCol; r += rowDir, c += colDir) {
        if (board.getElement(r, c) != nullptr) return false;
    }
    return true;
}


queen::queen(int row, int col, bool isWhite) : piece(row, col, isWhite ? 'Q' : 'q', isWhite) {}
bool queen::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (!piece::isvalidmove(newRow, newCol, board)) return false;
    bool isStraight = (m_row == newRow || m_col == newCol);
    bool isDiagonal = (std::abs(newRow - m_row) == std::abs(newCol - m_col));
    if (!isStraight && !isDiagonal) return false;

    int rowDir = (newRow == m_row) ? 0 : (newRow > m_row ? 1 : -1);
    int colDir = (newCol == m_col) ? 0 : (newCol > m_col ? 1 : -1);

    for (int r = m_row + rowDir, c = m_col + colDir; r != newRow || c != newCol; r += rowDir, c += colDir) {
        if (board.getElement(r, c) != nullptr) return false;
    }
    return true;
}


king::king(int row, int col, bool isWhite) : piece(row, col, isWhite ? 'K' : 'k', isWhite) {}
bool king::isvalidmove(int newRow, int newCol, const Matrix<PiecePtr>& board) const {
    if (!piece::isvalidmove(newRow, newCol, board)) return false;
    int rowDiff = std::abs(newRow - m_row);
    int colDiff = std::abs(newCol - m_col);

    
    if (rowDiff <= 1 && colDiff <= 1) return true;
    
    
    
    return false; 
}