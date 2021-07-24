#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "chess/chess.h"
#include "engine/minimax.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define SQUARE_SIZE (SCREEN_HEIGHT/8)

void initSDL(void);
void prepareScene(void);
void presentScene(void);
void handleClick(void);
void draw_circle(SDL_Point center, int radius, SDL_Color color);

typedef struct {
	SDL_Renderer * renderer;
	SDL_Window * window;
}App;

App app;

std::map<Piece, SDL_Texture*> pieceImage;
Chess chess;
std::vector<Piece> board = chess.getCurrentBoard();
std::vector<Move> moves = chess.getLegalMoves();
int selectedPiece = -1;
bool selectedMoves[64] = { 0 };

int main(int argc, char* argv[]) {
    SDL_SetMainReady();

	initSDL();

	// Pieces images
	pieceImage[W_PAWN] = IMG_LoadTexture(app.renderer, "./GUI/assets/white_pawn.png");
	pieceImage[W_KNIGHT] = IMG_LoadTexture(app.renderer, "./GUI/assets/white_knight.png");
	pieceImage[W_BISHOP] = IMG_LoadTexture(app.renderer, "./GUI/assets/white_bishop.png");
	pieceImage[W_ROOK] = IMG_LoadTexture(app.renderer, "./GUI/assets/white_rook.png");
	pieceImage[W_QUEEN] = IMG_LoadTexture(app.renderer, "./GUI/assets/white_queen.png");
	pieceImage[W_KING] = IMG_LoadTexture(app.renderer, "./GUI/assets/white_king.png");
	pieceImage[B_PAWN] = IMG_LoadTexture(app.renderer, "./GUI/assets/black_pawn.png");
	pieceImage[B_KNIGHT] = IMG_LoadTexture(app.renderer, "./GUI/assets/black_knight.png");
	pieceImage[B_BISHOP] = IMG_LoadTexture(app.renderer, "./GUI/assets/black_bishop.png");
	pieceImage[B_ROOK] = IMG_LoadTexture(app.renderer, "./GUI/assets/black_rook.png");
	pieceImage[B_QUEEN] = IMG_LoadTexture(app.renderer, "./GUI/assets/black_queen.png");
	pieceImage[B_KING] = IMG_LoadTexture(app.renderer, "./GUI/assets/black_king.png");

	Minimax mm;
	Evaluation evaluation;
	evaluation = mm.searchABPruning(chess, 3);
	std::cout << evaluation.result << std::endl;
	std::cout << (int)evaluation.move.squareFrom << std::endl;
	std::cout << (int)evaluation.move.squareTo << std::endl;

	bool run = true;
	while (run) {
		prepareScene();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				run = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					handleClick();
				}
				break;
			default:
				break;
			}
		}

		presentScene();
	}

    SDL_Quit();

	return 0;
}

void initSDL(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Couldn't initialize SDL:" << SDL_GetError() << std::endl;
	}

	app.window = SDL_CreateWindow("Balarama", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!app.window)
	{
		std::cout << "Failed to open window:" << SDL_GetError() << std::endl;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	if (!app.renderer)
	{
		std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
	}

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
}

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 211, 211, 211, 255);
	SDL_RenderClear(app.renderer);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if ((x + y) % 2 == 0) {
				SDL_SetRenderDrawColor(app.renderer, 245, 222, 179, 255);
			}
			else {
				SDL_SetRenderDrawColor(app.renderer, 111, 78, 55, 255);
			}

			SDL_Rect rect;
			rect.x = SQUARE_SIZE * x;
			rect.y = SQUARE_SIZE * y;
			rect.w = SQUARE_SIZE;
			rect.h = SQUARE_SIZE;

			SDL_RenderFillRect(app.renderer, &rect);

			int squareIndex = (7 - y) * 8 + x;

			if (board[squareIndex] != UNKNOWN) {
				SDL_RenderCopy(app.renderer, pieceImage[board[squareIndex]], NULL, &rect);
			}

			if (selectedPiece >= 0) {
				if (selectedPiece == squareIndex) {
					SDL_SetRenderDrawColor(app.renderer, 50, 205, 50, 255);
					SDL_RenderDrawRect(app.renderer, &rect);
				}
				else if (selectedMoves[squareIndex]) {
					SDL_Color green;
					green.r = 50;
					green.g = 205;
					green.b = 50;
					green.a = 255;
					SDL_Point point;
					point.x = rect.x + SQUARE_SIZE / 2;
					point.y = rect.y + SQUARE_SIZE / 2;
					draw_circle(point, (SQUARE_SIZE / 6), green);
				}
			}
		}
	}
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

void handleClick(void) {
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	if (mouseX < SCREEN_HEIGHT) {
		int x = mouseX / SQUARE_SIZE;
		int y = mouseY / SQUARE_SIZE;
		int clickedSquare = (7 - y) * 8 + x;

		// Own piece clicked, fill the posible moves.
		if (chess.colorTurn == chess.getSquareColor(clickedSquare)) {
			selectedPiece = clickedSquare;
			for (int i = 0; i < 64; i++) {
				selectedMoves[i] = false;
			}
			for (Move m : moves) {
				if ((int)m.squareFrom == clickedSquare) {
					selectedMoves[(int)m.squareTo] = true;
				}
			}
		}
		// If its not an owned piece check if its a valid move
		else if(selectedPiece >= 0){
			for (Move m : moves) {
				if (m.squareFrom == selectedPiece && m.squareTo == (Square)clickedSquare) {
					/*std::string pieces[(int)UNKNOWN + 1] = {
						"WHITE",      "BLACK",
						"W_PAWN",     "B_PAWN",
						"W_KNIGHT",   "B_KNIGHT",
						"W_BISHOP",   "B_BISHOP",
						"W_ROOK",     "B_ROOK",
						"W_QUEEN",    "B_QUEEN",
						"W_KING",     "B_KING",
						"UNKNOWN"
					};


					std::cout << "SquareFrom: " << (int)m.squareFrom << std::endl;
					std::cout << "SquareTo: " << (int)m.squareTo << std::endl;
					std::cout << "PieceColor: " << pieces[(int)m.pieceColor] << std::endl;
					std::cout << "PieceType: " << pieces[(int)m.pieceType] << std::endl;
					std::cout << "CPieceColor" << pieces[(int)m.cPieceColor] << std::endl;
					std::cout << "CPieceType" << pieces[(int)m.cPieceType] << std::endl;*/
					chess.makeMove(m);
					board = chess.getCurrentBoard();
					moves = chess.getLegalMoves();
					break;
				}
			}
			selectedPiece = -1;
		}
	}
}

void draw_circle(SDL_Point center, int radius, SDL_Color color)
{
	SDL_SetRenderDrawColor(app.renderer, color.r, color.g, color.b, color.a);
	for (int w = 0; w < radius * 2; w++)
	{
		for (int h = 0; h < radius * 2; h++)
		{
			int dx = radius - w; // horizontal offset
			int dy = radius - h; // vertical offset
			if ((dx * dx + dy * dy) <= (radius * radius))
			{
				SDL_RenderDrawPoint(app.renderer, center.x + dx, center.y + dy);
			}
		}
	}
}