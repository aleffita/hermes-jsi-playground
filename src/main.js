var ticks = 0;

function main(e) {

    ticks % 60 === 0 && e && print(e)
    ticks++;
    ticks % 60 === 0 && print( ticks / 60 + "  seconds" )
}

(() => {
    return {
        main: (...e) => setTimeout(() => main(e), 16)
    }
})();

