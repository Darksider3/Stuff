class Grid {
    constructor(canvas, ctx) {
        this.Size = {
            x: 400,
            y: 400
        };
        this.padding = 50;
        this.rects = 8;
        this.ctx = ctx;
        this.canvas = canvas;
        
        this.ctx.lineWidth = 1;
    }
    adjust(x, y, padding = 50) {
        this.Size.x = x;
        this.Size.y = y;
        this.padding = padding;
        this.rects = Math.floor(x / padding);
        return this;
    }

    draw() {
        for (let ycord = 0; ycord <= this.rects - 1; ycord += 1) {
            for (let xcord = 0; xcord <= this.rects - 1; xcord += 1) {
                this.ctx.strokeRect(xcord * this.padding, ycord * this.padding, this.padding, this.padding);
            }
        }

    }

    fillShape(x, y) {
        //floor it first so we get always the correct rectangle to work with
        x = Math.floor(x / this.padding) * this.padding;
        y = Math.floor(y / this.padding) * this.padding;
        if (x > this.Size.x - 1)
            return;
        if (y > this.Size.y - 1)
            return;
        let data = this.gridElementData(x, y)
        if (data[0] > 0 || data[1] > 0 || data[2] > 0 || data[3] > 244) {
            this.ctx.clearRect(x, y, this.padding, this.padding); //clear full rect
            this.ctx.strokeRect(x, y, this.padding, this.padding); //redraw rect
            return;
        }
        this.ctx.fillRect(x, y, this.padding, this.padding);
    }
    
    clearGrid() {
      for(let y = 0; y != this.canvas.height; y += 1)
      {
        for(let x = 0; x != this.canvas.width; x += 1)
        {
          let imgdata = this.ctx.getImageData(x,y, 1, 1);
          if(this.allzero(imgdata.data))
          {
            imgdata.data[0] = 256;
            imgdata.data[1] = 256;
            imgdata.data[2] = 256;
            imgdata.data[3] = 0;
            this.ctx.putImageData(imgdata, x, y);
          }
        }
      }
    }
    
    allzero(isZero)
    {
      for(let i = 0; i != isZero.length-1; i += 1)
      {
        if(isZero[i] != 0)
          return false;
      }
      return true;
    }
    
    gridElementData(x, y)
    {
      x = Math.floor(x / this.padding) * this.padding;
      y = Math.floor(y / this.padding) * this.padding;
      return this.ctx.getImageData(x, y, 1, 1).data;
    }
};

function getMousePosition(canvas, event) {
    let rect = canvas.getBoundingClientRect();
    let x = event.clientX - rect.left;
    let y = event.clientY - rect.top;
    console.log("Coordinate x: " + x,
        "Coordinate y: " + y);
    return Coords = {
        x: x,
        y: y
    };
}

function getRandomColor() {
    let letters = '0123456789ABCDEF';
    let color = '#';
    for (let i = 0; i < 6; i++) {
        color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
}

function downloadCanvas(link, canvasId, filename) {
    link.href = document.getElementById(canvasId).toDataURL();
    link.download = filename;
}

function eight(gridObj) {
    gridObj.adjust(8 * 50, 8 * 50, 50);
    gridObj.rects = 8;
    gridObj.ctx.clearRect(0, 0, canvas.width, canvas.height);
    gridObj.draw();
}

function sixteen(gridObj) {
    gridObj.adjust(16 * 25, 16 * 25, 25);
    gridObj.rects = 16;
    gridObj.ctx.clearRect(0, 0, canvas.width, canvas.height);
    gridObj.draw();
}

function thirtyTwo(gridObj) {
    gridObj.adjust(32 * 12, 32 * 12, 12);
    gridObj.rects = 32;
    gridObj.ctx.clearRect(0, 0, canvas.width, canvas.height);
    gridObj.draw();
}

