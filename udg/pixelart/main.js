let canvasElem = document.querySelector("canvas");
let canvas = document.getElementById("canvas");
let context = canvas.getContext("2d");

let mainGrid = new Grid(canvas, context);
mainGrid.adjust(8 * 50, 8 * 50);
mainGrid.rects = 8;
mainGrid.ctx = context;
mainGrid.ctx.fillStyle = "#000001"
mainGrid.draw();
canvasElem.addEventListener("mousedown", function(e) {
    let coords = getMousePosition(canvasElem, e);
    mainGrid.fillShape(coords.x, coords.y);
    mainGrid.ctx.fillStyle = getRandomColor();
});

document.getElementById('download').addEventListener('click', function() {
    mainGrid.clearGrid();
    downloadCanvas(this, 'canvas', 'test.png');
    mainGrid.draw();
}, false);

eight(mainGrid);
