#include "Renderer.h"
#include "ChessGame.h"
#include "embedded_resources.h"
#include "GameState.h"

constexpr int WINDOW_WIDTH = 1150;
constexpr int WINDOW_HEIGHT = 750;
constexpr float MOVE_CIRCLE = 15;

auto TITLE = "Chess";

Renderer::Renderer() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    SetTargetFPS(60);
    const Image piecesImage = LoadImageFromMemory(".png", Chess_Pieces_Sprite_png, Chess_Pieces_Sprite_png_size);
    piecesTexture = LoadTextureFromImage(piecesImage);
    UnloadImage(piecesImage);
}

Renderer::~Renderer() {
    UnloadTexture(piecesTexture);
    CloseWindow();
}

void Renderer::draw_end_game_state(ChessGame &game, const Vector2 mouse) const {
    for (auto &p: game.board().pieceList) {
        draw_piece(p);
    }
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {100, 100, 100, 150});
    if (game.state().state == GameStatus::Checkmate) {
        const std::string text = game.state().winner == PieceColor::White ? "White wins!" : "Black wins!";
        DrawText("Checkmate!", 300, 300, 30, WHITE);
        DrawText(text.c_str(), 300, 330, 30, WHITE);
    } else {
        DrawText("Stalemate!", 300, 300, 30, WHITE);
    }

    DrawRectangle(260, 380, 225, 60, BLACK);
    DrawRectangle(265, 385, 215, 50, WHITE);
    DrawText("Restart", 325, 400, 28, BLACK);

    if (CheckCollisionPointRec(mouse, {260, 380, 225, 60}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game.restart_game(piecesTexture);
    }
}

void Renderer::draw_board() {
    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            const Color colour = (i + j) % 2 == 0 ? WHITE : GRAY;
            DrawRectangle(SQUARE_SIZE * j, SQUARE_SIZE * i, SQUARE_SIZE, SQUARE_SIZE, colour);
        }
    }
    for (int i = 1; i <= 8; i++) {
        const char *letters[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
        DrawText(letters[i - 1], SQUARE_SIZE * i + 30, 680, 16, WHITE);
    }

    for (int i = 1; i <= 8; i++) {
        const char *numbers[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
        DrawText(numbers[i - 1], 60, SQUARE_SIZE * i + 30, 16, WHITE);
    }
}

void Renderer::draw_pieces(ChessGame &game, const RenderState render_state) const {
    const Piece *held_piece = render_state.currentPiece;

    // Draw all the pieces
    for (const auto p: game.board().pieceList) {
        if (p.captured) continue;
        if (&p == held_piece) continue;
        draw_piece(p);
    }

    // Draw Held piece and its legal moves
    if (held_piece) {
        for (const std::string &move: held_piece->legalMoves) {
            draw_legal_moves(game.board(), move, held_piece->colour);
        }
        draw_piece(*held_piece);
    }

    // Draw turn indicator
    if (game.state().turn == PieceColor::White)
        DrawText("White's turn", 10, 10, 20, WHITE);
    else
        DrawText("Black's turn", 10, 10, 20, WHITE);
}

void Renderer::draw(ChessGame &game, const Vector2 &mouse, const RenderState render_state) const {
    ClearBackground(DARKBROWN);

    // Draw game select
    if (game.state().state == GameStatus::Menu) {
        draw_menu(game, mouse);
        return;
    }

    draw_board();
    draw_taken_material(game.board());
    draw_move_history(game.state().move_history);

    // Game end loop
    if (game.state().state == GameStatus::Checkmate || game.state().state == GameStatus::Stalemate) {
        draw_end_game_state(game, mouse);
        return;
    }

    draw_pieces(game, render_state);
}

void Renderer::draw_button(const Rectangle rect, const Color color, const char *text) {
    constexpr int font_size = 25;
    constexpr int border = 5;
    const auto [x, y] = MeasureTextEx(GetFontDefault(), text, font_size, 0);
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
    DrawRectangle(rect.x + border, rect.y + border, rect.width - border * 2, rect.height - border * 2, color);
    const Vector2 button_center = {rect.x + rect.width / 2, rect.y + rect.height / 2};
    const Vector2 text_point = {button_center.x - x / 2, button_center.y - y / 2};
    DrawText(text, text_point.x, text_point.y, font_size, BLACK);
}

void Renderer::draw_menu(ChessGame &game, const Vector2 mouse) {
    constexpr int button_w = 250;
    constexpr int button_y = 300;
    constexpr int bot_button_y = 400;
    constexpr int button_h = 50;
    constexpr Rectangle two_player_btn = {WINDOW_WIDTH / 2 - button_w / 2, button_y, button_w, button_h};
    constexpr Rectangle bot_game = {WINDOW_WIDTH / 2 - button_w / 2, bot_button_y, button_w, button_h};

    draw_button(two_player_btn, WHITE, "Two player");
    draw_button(bot_game, WHITE, "Bot game");

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, two_player_btn)) {
            game.state().bot_game = false;
            game.state().state = GameStatus::Normal;
        }
        if (CheckCollisionPointRec(mouse, bot_game)) {
            game.state().bot_game = true;
            game.state().state = GameStatus::Normal;
        }
    }
}

void Renderer::draw_legal_moves(const Board &board, const std::string &notation, const PieceColor colour) {
    const auto [x, y, width, height] = board.squares.at(notation).squareBox;
    if (board.squares.at(notation).piece != nullptr && board.squares.at(notation).piece->colour != colour) {
        DrawCircle(x + width / 2, y + height / 2, MOVE_CIRCLE, {100, 100, 100, 150});
    }
    if (board.squares.at(notation).piece == nullptr) {
        DrawCircle(x + width / 2, y + height / 2, MOVE_CIRCLE, {100, 100, 100, 150});
    }
}

void Renderer::draw_move_history(const MoveHistory &move_history) {
    constexpr int border = 5;
    const int moves_count = move_history.movesList.size();

    int move_offset = 0;
    if (moves_count > MOVES_DRAW_LIMIT) {
        move_offset = (moves_count / MOVES_DRAW_LIMIT - 1) * MOVES_DRAW_LIMIT;
        move_offset += moves_count % MOVES_DRAW_LIMIT;
        move_offset % 2 == 0 ? move_offset : move_offset++; // round offset to even number
    }

    DrawRectangle(750, 75, 300, 600, BLACK);
    DrawRectangle(750 + border, 75 + border, 300 - border * 2, 600 - border * 2, GRAY);
    int text_offset_y = 0;
    int text_offset_x = 0;

    for (size_t i = move_offset; i < moves_count; i++) {
        const int move = i / 2 + 1;
        if (i % 2 == 0) {
            auto number_str = std::to_string(move) + ".";
            DrawText(number_str.c_str(), 790, 100 + text_offset_y, 20, BLACK);
            DrawText(move_history.movesList[i].c_str(), 850 + text_offset_x, 100 + text_offset_y, 20, BLACK);
            text_offset_x = 100;
        } else {
            DrawText(move_history.movesList[i].c_str(), 850 + text_offset_x, 100 + text_offset_y, 20, BLACK);
            text_offset_y += 30;
            text_offset_x = 0;
        }
    }
}

void Renderer::draw_taken_material(const Board &board) const {
    constexpr Vector2 white_taken = {80, 40};
    constexpr Vector2 black_taken = {80, 700};
    int white_taken_idx = 0;
    int black_taken_idx = 0;
    int white_material_taken = 0;
    int black_material_taken = 0;
    constexpr int gap = 20;

    for (const auto &piece: board.pieceList) {
        if (piece.captured) {

            if (piece.colour == PieceColor::White) {
                black_material_taken += piece.value;
                DrawTexturePro(piecesTexture, piece.pieceTexture,
                               {white_taken.x + gap * white_taken_idx, white_taken.y, 25, 25}, {0, 0}, 0, WHITE);
                white_taken_idx++;
            } else {
                DrawTexturePro(piecesTexture, piece.pieceTexture,
                               {black_taken.x + gap * black_taken_idx, black_taken.y, 25, 25}, {0, 0}, 0, WHITE);
                white_material_taken += piece.value;
                black_taken_idx++;
            }
        }
    }
    const int white_advantage = white_material_taken - black_material_taken;
    const int black_advantage = black_material_taken - white_material_taken;
    const std::string white_text = "+" + std::to_string(white_advantage);
    const std::string black_text = "+" + std::to_string(black_advantage);
    if (white_advantage > 0)
        DrawText(white_text.c_str(), black_taken.x + gap * black_taken_idx, black_taken.y + 5, 20,
                 WHITE);
    if (black_advantage > 0)
        DrawText(black_text.c_str(), white_taken.x + gap * white_taken_idx, white_taken.y + 5, 20,
                 WHITE);
}

void Renderer::draw_piece(const Piece& piece) const {
    if (!piece.captured) {
        DrawTexturePro(piecesTexture, piece.pieceTexture, piece.textureDest, {0, 0}, 0, WHITE);
    } else {
        DrawTexturePro(piecesTexture, piece.pieceTexture, piece.textureDest, {0, 0}, 0, BLANK);
    }
}