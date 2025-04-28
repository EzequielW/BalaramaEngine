import createBalarama from '/balarama.js';

let Module = null
let Chess = null
let Engine = null
let legalMoves = null
const depth = 6

const getEvaluation = () => {
    const evaluation = Engine.searchABPruning(Chess, depth)
    let pieceChar = String.fromCharCode(Module.pieceToString(evaluation.move.pieceType))
    pieceChar = pieceChar === 'p' || pieceChar === 'P' ? '' : pieceChar
    const bestMove = {
        from: Module.squareToString(evaluation.move.squareFrom),
        to: Module.squareToString(evaluation.move.squareTo),
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
        newMove.castleFrom = 'a1'
        newMove.castleTo = 'a1'

        // for rook movement during castle
        if(newMove.flags === 'k') {
            newMove.castleFrom = newMove.color === 'w' ? 'h1' : 'h8'
            newMove.castleTo = newMove.color === 'w' ? 'f1' : 'f8'
            newMove.captured = 'r'
        }
        else if(newMove.flags === 'q') {  
            newMove.castleFrom = newMove.color === 'w' ? 'a1' : 'a8'
            newMove.castleTo = newMove.color === 'w' ? 'd1' : 'd8'
            newMove.captured = 'r'
        }

        const validMove = legalMoves.find(m => {
            const from = Module.squareToString(m.squareFrom)
            const to = Module.squareToString(m.squareTo)
            const castleFrom = Module.squareToString(m.castleFrom)
            const castleTo = Module.squareToString(m.castleTo)
            const pieceCaptured = String.fromCharCode(Module.pieceToString(m.cPieceType)).toLowerCase()
            const isCapture = pieceCaptured === '-' ? true : pieceCaptured === newMove.captured

            // console.log('---------')
            // console.log('from: ', from)
            // console.log('to: ', to)
            // console.log('castleFrom: ', castleFrom)
            // console.log('castleTo: ', castleTo)
            // console.log('pieceCaptured: ', pieceCaptured)
    
            return from === newMove.from && to === newMove.to && isCapture 
                && castleFrom === newMove.castleFrom && castleTo === newMove.castleTo
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