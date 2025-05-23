import createBalarama from '/balarama.js';

let Module = null
let Chess = null
let Engine = null
let legalMoves = null
const depth = 4

const getEvaluation = () => {
    const evaluation = Engine.searchABPruning(Chess, depth)
    const move = Module.getJSMove(evaluation.move)
    const pieceType = Chess.getPieceAt(move.from)
    let pieceChar = String.fromCharCode(Module.pieceToString(pieceType))
    pieceChar = pieceChar === 'p' || pieceChar === 'P' ? '' : pieceChar

    const bestMove = {
        from: Module.squareToString(move.from),
        to: Module.squareToString(move.to),
        piece: pieceChar.toUpperCase()
    }

    return {
        evaluation: evaluation,
        bestMove: bestMove
    }
}

self.onmessage = async function (e) {
    //   const { fen } = e.data
    const { newMove, fen, skipEval } = e.data
 
    if (!Module) {
        Module = await createBalarama()
    }

    if (!Chess) {
        Chess = new Module.Chess()
        // chess.load_fen(fen)
    }

    if (!Engine) {
        Engine = new Module.Minimax()
    }

    if (fen) {
        Chess = new Module.Chess()
    }

    if (newMove && legalMoves) {
        let flag = Module.MoveFlag.QUIET_MOVE

        // for rook movement during castle
        if(newMove.flags === 'b') {
            flag = Module.MoveFlag.DOUBLE_PAWN
        }
        else if(newMove.flags === 'k') {
            flag = Module.MoveFlag.KING_CASTLE
        }
        else if(newMove.flags === 'q') {  
            flag = Module.MoveFlag.QUEEN_CASTLE
        }
        else if(newMove.flags === 'e') {
            flag = Module.MoveFlag.EP_CAPTURE
        }
        else if(newMove.flags === 'np') {
            if(newMove.promotion === 'n') {
                flag = Module.MoveFlag.KNIGHT_PROMOTION
            }
            else if(newMove.promotion === 'b') {
                flag = Module.MoveFlag.BISHOP_PROMOTION
            }
            if(newMove.promotion === 'r') {
                flag = Module.MoveFlag.ROOK_PROMOTION
            }
            if(newMove.promotion === 'q') {
                flag = Module.MoveFlag.QUEEN_PROMOTION
            }
        }
        else if(newMove.flags === 'cp') {
            if(newMove.promotion === 'n') {
                flag = Module.MoveFlag.KNIGHT_PROMOTION_C
            }
            else if(newMove.promotion === 'b') {
                flag = Module.MoveFlag.BISHOP_PROMOTION_C
            }
            if(newMove.promotion === 'r') {
                flag = Module.MoveFlag.ROOK_PROMOTION_C
            }
            if(newMove.promotion === 'q') {
                flag = Module.MoveFlag.QUEEN_PROMOTION_C
            }
        }
        else if(newMove.captured) {
            flag = Module.MoveFlag.CAPTURE_MOVE
        }

        const validMove = legalMoves.find(m => {
            const jsMove = Module.getJSMove(m)
            const from = Module.squareToString(jsMove.from)
            const to = Module.squareToString(jsMove.to)
    
            return from === newMove.from && to === newMove.to && flag == jsMove.flags
        })
    
        // console.log('newMove: ', validMove)
    
        if(validMove) {
            Chess.makeMove(validMove)
            legalMoves = Chess.getLegalMovesAsJsArray()
            console.log('fen: ', Chess.getFen())

            if(skipEval) {
                self.postMessage({ finished: true })
            }
            else {
                const evaluation = getEvaluation()
                self.postMessage({
                    ...evaluation,
                    finished: true
                })
            }
        }
    }
    else {
        legalMoves = Chess.getLegalMovesAsJsArray()
        console.log('fen: ', Chess.getFen())

        const evaluation = getEvaluation()
        self.postMessage({
            ...evaluation,
            finished: true
        })
    }
}