import type { Config } from 'tailwindcss'

export default <Partial<Config>>{
    theme: {
        extend: {
            animation: {
                fadeIn: 'fadeIn 1.5s ease-out forwards',
            },
            keyframes: {
                fadeIn: {
                    '0%': { opacity: 0 },
                    '100%': { opacity: 1 },
                },
            },
        },
    }
}