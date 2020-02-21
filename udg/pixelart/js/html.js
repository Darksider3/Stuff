function GenericGrid(Rec, SVG, multiplier = 2, gridpxsize = 400) {
    Rec.clear();
    Rec.adjust(8 * multiplier, 37.5 * multiplier);
    SVG.adjust(8 * multiplier, gridpxsize);
  }
  
  function EightGrid() {
    Rec.clear();
    Rec.adjust(8, 37.5);
    SVG.adjust(8, 400);
  }
  
  function SixteenGrid() {
    Rec.clear();
    Rec.adjust(16, 18.75);
    SVG.adjust(16, 400);
  }
  
  function fillFunc(element) {
    if(Rec.FloodFill)
    {
      Rec.FloodFill = false;
      element.textContent = "Flood Fill! "
    }
    else {
      Rec.FloodFill = true;
      element.textContent = "Pixel Fill!";
    }
  }
  
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
  
  function getRandomColor() {
    let letters = "0123456789ABCDEF",
      color = "#";
    for (let i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
  }
  
  function getRandomColorRGBA() {
    let color = "rgba(";
    for (let i = 0; i < 3; ++i) {
      color += Math.floor(Math.random() * 256);
      color += ",";
    }
    color += "256)";
    return color;
  }
  
  function invertColor(hex, bw) {
    if (hex.indexOf('#') === 0) {
        hex = hex.slice(1);
    }
    // convert 3-digit hex to 6-digits.
    if (hex.length === 3) {
        hex = hex[0] + hex[0] + hex[1] + hex[1] + hex[2] + hex[2];
    }
    if (hex.length !== 6) {
        throw new Error('Invalid HEX color.');
    }
    var r = parseInt(hex.slice(0, 2), 16),
        g = parseInt(hex.slice(2, 4), 16),
        b = parseInt(hex.slice(4, 6), 16);
    if (bw) {
        // http://stackoverflow.com/a/3943023/112731
        return (r * 0.299 + g * 0.587 + b * 0.114) > 186
            ? '#000000'
            : '#FFFFFF';
    }
    // invert color components
    r = (255 - r).toString(16);
    g = (255 - g).toString(16);
    b = (255 - b).toString(16);
    // pad each with zeros and return
    return "#" + padZero(r) + padZero(g) + padZero(b);
  }
  
  
  function downloadCanvas(link, canvasId, filename, imgtype="image/png") {
    link.href = canvasId.toDataURL(imgtype);
    link.download = filename;
  }