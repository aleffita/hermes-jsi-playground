var sec = 0;

function mainloop() {

    sec++;
    sec % 60 === 0 && print( sec / 60 + "  segundos" )

    setTimeout(mainloop, 16);
}

print("Starting mainloop");
setTimeout(mainloop, 16);
