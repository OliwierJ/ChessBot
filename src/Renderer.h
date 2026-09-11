#ifndef CHESSBOT_RENDERER_H
#define CHESSBOT_RENDERER_H
#include "GameState.h"
#include "raylib.h"

class Piece;
class ChessGame;

struct RenderState {
    bool mouseLHeld = false;
    const Piece *currentPiece = nullptr;
};

class Renderer {
public:
    Texture2D piecesTexture;

    explicit Renderer();

    ~Renderer();

    // Render all components
    void draw(ChessGame &game, const Vector2 &mouse, RenderState render_state) const;

private:
    void draw_end_game_state(ChessGame &game, Vector2 mouse) const;

    void draw_pieces(ChessGame &game, RenderState render_state) const;

    static void draw_menu(ChessGame &game, Vector2 mouse);

    static void draw_legal_moves(const Board &board, const std::string &notation, PieceColor colour);

    static void draw_move_history(const MoveHistory &move_history);

    void draw_taken_material(const Board &board) const;

    void draw_piece(const Piece &piece) const;

    static void draw_board();

    static void draw_button(Rectangle rect, Color color, const char *text);
};

#endif //CHESSBOT_RENDERER_H
