canvasElem.addEventListener("mousedown", function (e) {
    let coords = getMousePosition(canvasElem, e);
    if (!CONFIG.customColor)
        Rec.fillStyle = getRandomColorRGBA()
    Rec.fillShape(coords.x, coords.y);
    dragged = true;
    Rec.setPrevious(coords.x, coords.y);
    canvasElem.addEventListener("mousemove", draggedDraw);
});

canvasElem.addEventListener("mouseup", function (e) {
    dragged = false;
    canvasElem.removeEventListener("mousemove", draggedDraw);
});
let ColorPicker = document.getElementById("kb_selected_color");
ColorPicker.addEventListener("input", function () {

    // Farcode (Hex) schreiben
    document.getElementById("kb_selected_color").style.background = ColorPicker.value;
    CONFIG.customColor = ColorPicker.value;
    Rec.fillStyle = CONFIG.customColor;
}, false);

document.getElementById('png').addEventListener('click', function () {
    downloadCanvas(this, canvasElem, 'art.png', "image/png");
}, false);

document.getElementById('jpg').addEventListener('click', function () {
    downloadCanvas(this, canvasElem, 'art.jpg', "image/jpeg");
}, false);


document.getElementById('bmp').addEventListener('click', function () {
    downloadCanvas(this, canvasElem, 'art.bmp', "image/bmp");
}, false);