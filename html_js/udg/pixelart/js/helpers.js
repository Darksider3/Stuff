/**
 * @typedef {Object} Coordinates
 * @property {Number} x - X-Coordinate.
 * @property {Number} y - Y-Coordinate.
 */
/**
 * Create a "generic" Grid(currently just SameXsame-Size, and 8, 16, 32)
 *
 * @param {Rects} Rec Rects-Object 
 * @param {SVGGrid} SVG SVG-Object
 * @param {number} [multiplier=2] Rectangle Count/8
 * @param {number} [gridpxsize=400] Size of the grid(400px standard)
 */
function GenericGrid(Rec, SVG, multiplier = 2, gridpxsize = 400) {
  Rec.clear();
  Rec.adjust(8 * multiplier, 37.5 / multiplier);
  SVG.adjust(8 * multiplier, gridpxsize);
}
/**
 * Handles pixel and/or flood filling toggle
 *
 * @param {Element} element Toggle-Button/Link DOM-Element
 */
function fillFunc(element) {
  if (Rec.FloodFill) {
    Rec.FloodFill = false;
    element.textContent = "Flood Fill! "
  }
  else {
    Rec.FloodFill = true;
    element.textContent = "Pixel Fill!";
  }
}

/**
 * Returns current relative mouse position inside the canvas
 *
 * @param {Element} canvas Canvas element to look for
 * @param {DocumentEvent} event DOM-Event which holds the mouse coordinates
 * @returns {Coordinates} Coordinates relative inside the canvas 
 */
function getMousePosition(canvas, event) {
  let rect = canvas.getBoundingClientRect(),
    x = event.clientX - rect.left,
    y = event.clientY - rect.top,
    Coords = { x: 0, y: 0 };
  DEBUG("Coordinate x: " + x, "Coordinate y: " + y);
  return (Coords = {
    x: x,
    y: y
  });
}
/**
 * Function to handle dragged drawing/filling events
 *
 * @param {DocumentEvent} e DOM-Event
 */
function draggedDraw(e) {
  if (dragged) {
    // safety ftw!
    let moveCoords = getMousePosition(canvasElem, e);
    if (Rec.isSameRect(moveCoords.x, moveCoords.y) != true) {
      Rec.fillShape(moveCoords.x, moveCoords.y);
      Rec.setPrevious(moveCoords.x, moveCoords.y);
    }
  }
}
/**
 * Generates a random color
 *
 * @returns {string} Random Color in Hex-format(#000000)
 */
function getRandomColor() {
  let letters = "0123456789ABCDEF",
    color = "#";
  for (let i = 0; i < 6; i++) {
    color += letters[Math.floor(Math.random() * 16)];
  }
  return color;
}
/**
 * Generate a random CSS3-compatible color in RGBA-format 
 * with opacity set to 256
 *
 * @returns {string} `rgba(....)`-String
 * @example getRandomColorRGBA(); // -> "rgba(12, 54, 1, 256)""
 */
function getRandomColorRGBA() {
  let color = "rgba(";
  for (let i = 0; i < 3; ++i) {
    color += Math.floor(Math.random() * 256);
    color += ",";
  }
  color += "256)";
  return color;
}

/**
 * Generates an data-url for a picture based on a given mimetype
 *
 * @param {Element} link Link which shall point to the ressource 
 * @param {Element} canvasId Canvas-Element that you want to convert
 * @param {string} filename Filename for the picture
 * @param {string} [imgtype="image/png"] Mimetype of the data-url(currently gif is not allowed inside FF and IE)
 */
function downloadCanvas(link, canvasId, filename, imgtype = "image/png") {
  link.href = canvasId.toDataURL(imgtype);
  link.download = filename;
}
