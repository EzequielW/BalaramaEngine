<template>
    <div
        class="min-h-screen bg-gradient-to-br from-gray-800 via-gray-900 to-black text-white flex flex-col items-center justify-center p-4 space-y-6">
        <canvas ref="canvasRef" class="absolute inset-0 w-full h-full pointer-events-none"></canvas>
        
        <div class="relative z-10 bg-gray-700 p-6 rounded-2xl shadow-2xl w-full max-w-xl space-y-4">
            <h1 class="text-4xl md:text-5xl text-center font-bold text-white drop-shadow-lg mb-8 animate-fadeIn">
                Balarama Engine ♛
            </h1>

            <div class="flex justify-between items-center">
                <UButton class="cursor-pointer" @click="toggleMode" :disabled="engineLoading" size="xl">
                    Switch to {{ mode === 'play-ai' ? 'Analysis Mode' : 'Play vs AI' }}
                </UButton>
                <UButton class="cursor-pointer" @click="resetBoard" :disabled="engineLoading" size="xl" color="error">
                    Reset Board
                </UButton>
            </div>

            <TheChessboard @board-created="(api) => (boardAPI = api)" :board-config="boardConfig"
                boardClass="rounded-lg shadow-md" style="width: 100%;" @move="onMove" />

            <div class="text-center mt-4">
                <UProgress :animation="engineLoading ? 'swing' : undefined" v-model="engineProgress" />
                <div v-if="engineLoading" class="mt-2 text-lg">
                    Thinking...
                </div>
                <div v-if="evaluation && !engineLoading && mode === 'analysis'" class="mt-2 text-lg">
                    <div>Best Move: {{ bestMove.piece + bestMove.from }} → {{ bestMove.to }}</div>
                    <div>Eval: {{ (evaluation.result > 0 ? '+' : '') + evaluation.result.toFixed(2) }}</div>
                </div>

            </div>
        </div>

        <div class="absolute bottom-4 w-full flex justify-center z-10">
            <p class="text-gray-400 text-sm font-light">
                Made with ♟️ by <span class="text-white font-semibold">Ezequiel Baez</span>
            </p>
        </div>
    </div>
</template>

<script setup>
import { TheChessboard  } from 'vue3-chessboard';
import 'vue3-chessboard/style.css';

let engineWorker = null
let boardAPI = null
const fen = ref('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
const evaluation = ref()
const bestMove = ref()
const engineLoading = ref(true)
const mode = ref('play-ai')
const colorTurn = ref('white')

const canvasRef = ref(null)

let particles = []
let animationFrameId
const chessPieces = ['♔', '♕', '♖', '♗', '♘', '♙', '♚', '♛', '♜', '♝', '♞', '♟']

const boardConfig = computed(() => {
    return {
        // fen: fen.value,
        orientation: 'white',
        coordinates: false,
        viewOnly: colorTurn.value === 'black' && mode.value === 'play-ai'
    }
})

const engineProgress = computed(() => {
    return engineLoading.value ? undefined : 100
})

const onMove = (move) => {
    colorTurn.value = move.color === 'w' ? 'black' : 'white'

    // console.log('move: ', move)

    engineLoading.value = true
    if(mode.value === 'play-ai' && colorTurn.value === 'white') {
        engineWorker.postMessage({ newMove: move, skipEval: true })
    }
    else {
        engineWorker.postMessage({ newMove: move })
    }
}

function toggleMode() {
    mode.value = mode.value === 'play-ai' ? 'analysis' : 'play-ai';
}

const resetBoard = () => {
    boardAPI.resetBoard()
    engineLoading.value = true
    engineWorker.postMessage({ fen: 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' })
}

watch(boardConfig, () => {
    boardAPI.setConfig(boardConfig.value)
})

onMounted(() => {
    engineWorker = new Worker('/engine-worker.js', { type: 'module' })

    engineWorker.onmessage = (e) => {
        if (e.data.evaluation && e.data.bestMove) {
            evaluation.value = e.data.evaluation
            bestMove.value = e.data.bestMove   

            if (colorTurn.value === 'black' && mode.value === 'play-ai') {
                boardAPI.move({
                    from: bestMove.value.from,
                    to: bestMove.value.to
                })
            }
        }

        if (e.data.finished) {
            engineLoading.value = false
        }
    }
    engineLoading.value = true
    engineWorker.postMessage({})

    // Background effects
    const canvas = canvasRef.value
    if (!canvas) return
    const ctx = canvas.getContext('2d')
    if (!ctx) return

    const resize = () => {
        canvas.width = window.innerWidth
        canvas.height = window.innerHeight
    }
    window.addEventListener('resize', resize)
    resize();

    // Create particles
    for (let i = 0; i < 60; i++) {
        particles.push({
            x: Math.random() * canvas.width,
            y: Math.random() * canvas.height,
            vx: (Math.random() - 0.5) * 0.4,
            vy: (Math.random() - 0.5) * 0.4,
            piece: chessPieces[Math.floor(Math.random() * chessPieces.length)],
            size: Math.random() * 24 + 16,
        })
    }

    const animate = () => {
        if (!ctx) return;
        ctx.clearRect(0, 0, canvas.width, canvas.height)

        for (const p of particles) {
            p.x += p.vx
            p.y += p.vy

            // Bounce off edges
            if (p.x <= 0 || p.x >= canvas.width) p.vx *= -1
            if (p.y <= 0 || p.y >= canvas.height) p.vy *= -1

            ctx.font = `${p.size}px serif`
            ctx.fillStyle = 'rgba(255, 255, 255, 0.5)'
            ctx.fillText(p.piece, p.x, p.y)
        }

        animationFrameId = requestAnimationFrame(animate)
    };

    animate();

    onUnmounted(() => {
        cancelAnimationFrame(animationFrameId)
        window.removeEventListener('resize', resize)
    })
})
</script>