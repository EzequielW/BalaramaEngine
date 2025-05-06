#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <chrono>
#include <memory>

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
void updateEvalTexts(void);
void updateEval(void);
void doPerft(void);

typedef struct {
	SDL_Renderer * renderer;
	SDL_Window * window;
}App;

App app;

std::map<Piece, SDL_Texture*> pieceImage;
Chess chess;
Minimax mm;
FinalEvaluation evaluation;
std::vector<Piece> board = chess.getCurrentBoard();
MoveList moves = chess.getLegalMoves();
int selectedPiece = -1;
bool selectedMoves[64] = { 0 };

const char* SquareText[] =
{
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

TTF_Font* Sans;
SDL_Color textColor = { 255, 255, 255 };
SDL_Texture* evalText;
SDL_Rect evalRect;
SDL_Texture* moveText;
SDL_Rect moveRect;

bool updatingEval = false;
bool updateEvalRequest = false;
int evalCounter = 0;
int currentEval = 0;

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

	Sans = TTF_OpenFont("Sans.ttf", 36);

	updatingEval = true;
	updateEvalTexts();
	std::thread evalThread(updateEval);
	evalThread.detach();
	// std::thread perftThread(doPerft);
	// perftThread.detach();

	bool run = true;
	while (run) {
		if (updateEvalRequest && !updatingEval) {
			updatingEval = true;
			updateEvalTexts();

			std::thread newEvalThread(updateEval);
			newEvalThread.detach();
			updateEvalRequest = false;
		}

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
	TTF_Init();
}

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 211, 211, 211, 255);
	SDL_RenderClear(app.renderer);

	// Draw chessboard
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

	// Draw board evaluation
	if (!updatingEval && currentEval != evalCounter) {
		updateEvalTexts();
		currentEval = evalCounter;
	}

	SDL_RenderCopy(app.renderer, moveText, NULL, &moveRect);
	SDL_RenderCopy(app.renderer, evalText, NULL, &evalRect);
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
				if ((int)m.getFrom() == clickedSquare) {
					selectedMoves[(int)m.getTo()] = true;
				}
			}
		}
		// If its not an owned piece check if its a valid move
		else if(selectedPiece >= 0){
			for (Move m : moves) {
				if (m.getFrom() == selectedPiece && m.getTo() == (Square)clickedSquare) {
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
					std::cout << "Fen value: " << chess.getFen() << std::endl;
					updateEvalRequest = true;
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

void updateEvalTexts() {
	char evalBuffer[10];
	snprintf(evalBuffer, sizeof evalBuffer, "%03.2f", evaluation.result);
	const char* moveBuffer = SquareText[evaluation.move.getTo()];
	char movePiece[5] = {};
	switch (chess.pieceAt[evaluation.move.getFrom()]) {
	case W_BISHOP:
	case B_BISHOP:
		movePiece[0] = 'B';
		break;
	case W_KNIGHT:
	case B_KNIGHT:
		movePiece[0] = 'N';
		break;
	case W_ROOK:
	case B_ROOK:
		movePiece[0] = 'R';
		break;
	case W_KING:
	case B_KING:
		movePiece[0] = 'K';
		break;
	case W_QUEEN:
	case B_QUEEN:
		movePiece[0] = 'Q';
		break;
	default:
		break;
	}

	if (evaluation.move.getFlags() == CAPTURE_MOVE) {
		movePiece[1] = 'x';
	}

	strcat_s(movePiece, moveBuffer);

	SDL_Surface* evalSurface = TTF_RenderText_Solid(Sans, updatingEval ? "...loading" : evalBuffer, textColor);
	evalText = SDL_CreateTextureFromSurface(app.renderer, evalSurface);
	evalRect.x = SCREEN_HEIGHT + (SCREEN_WIDTH - SCREEN_HEIGHT) / 2 - evalSurface->w / 2;
	evalRect.y = 100;
	evalRect.w = evalSurface->w;
	evalRect.h = evalSurface->h;
	SDL_FreeSurface(evalSurface);

	SDL_Surface* moveSurface = TTF_RenderText_Solid(Sans, updatingEval ? "...loading" : movePiece, textColor);
	moveText = SDL_CreateTextureFromSurface(app.renderer, moveSurface);
	moveRect.x = SCREEN_HEIGHT + (SCREEN_WIDTH - SCREEN_HEIGHT) / 2 - moveSurface->w / 2;
	moveRect.y = 200;
	moveRect.w = moveSurface->w;
	moveRect.h = moveSurface->h;
	SDL_FreeSurface(moveSurface);
}

void updateEval() {
	auto t1 = std::chrono::high_resolution_clock::now();

	Chess chessboardCopy = chess;
	evaluation = mm.searchABPruning(chessboardCopy, 7);

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

	std::cout << evaluation.steps / (ms_int.count()) << " knodes\n";
	std::cout << ms_int.count() << "ms\n";
	std::cout << evaluation.steps << " steps\n";
	std::cout << evaluation.heuristicTime / 1000 << "ms heuristic\n";
	std::cout << evaluation.moveGenTime / 1000 << "ms move gen\n\n";

	// for(Move m : evaluation.moveTree) {
	// 	if(m.move == 0) {
	// 		continue;
	// 	}
	// 	Square from = (Square)m.getFrom();
	// 	Square to = (Square)m.getTo();

	// 	std::cout << "\nMove from: " + squareToString(from) + ", to: " + squareToString(to) << std::endl;
	// }

	updatingEval = false;
	evalCounter++;
}

void doPerft() {
	std::cout << "\nCalculating perft performance...\n" << std::endl;
	int depth = 5;
	Chess chessboardCopy = chess;
	PerftResults results = chessboardCopy.perft(depth);
	std::cout << "\nPerft results depth: " + std::to_string(depth) + "\n" << std::endl;
	std::cout << "\nNodes: " + std::to_string(results.totalCount) + "\n" << std::endl;
	std::cout << "\nCaptures: " + std::to_string(results.captures) + "\n" << std::endl;
	std::cout << "\nCheckmates: " + std::to_string(results.checkmates) + "\n" << std::endl;
}