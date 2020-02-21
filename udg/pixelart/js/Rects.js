/*
 * MIT License
 *
 * Copyright (c) 2020 darksider3 < github@darksider3.de >
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** 
 * @file Manages Rectangles as Pixels
 * @author darksider3 <github@darksider3.de>
*/

/**
 * @typedef {Object} Coordinates
 * @property {Number} x - X-Coordinate.
 * @property {Number} y - Y-Coordinate.
 */

/**
 * @typedef {Object} Indicies
 * @property {Number} x - X.
 * @property {Number} y - Y.
 */

"use strict";
/**
 * Rects manages the full behaivour of the drawn rectangles("pixels").
 */
const Rects = {

  /**
   * Initializes & sets all needed variables to draw the rectangles(constructor, but... without classes.) 
   *
   * @constructor
   * @param {Object canvas} ctx Initialised Canvas context
   * @param {number} [count=8]  Count of rectangles that fit in x and y
   * @param {number} [RectSize=37.5] Real size of the rectangles/2
   * @param {number} [PxSize=300] Real size of the canvas element drawing on
   * @param {string} [color="#29FF81"] Initial drawing color
   * @returns {Rects.Object} Object referencing this class/const object
   */
  init: function (ctx, count = 8, RectSize = 37.5, PxSize = 300, color = "#29FF81") {
    /** @private */ this.px = PxSize;
    /** @private */ this.ctx = ctx;
    /** @private */ this.RectCount = count;
    /** @private */ this.RectSize = RectSize;
    /** @private */ this.Rects = createArray(this.RectCount, this.RectCount);
    /** @private */ this.drawn = false;
    /** @public */ this.FloodFill = false;
    /** @public */ this.fillStyle = color;
    this.Size = {
      x: this.RectSize * this.RectCount,
      y: this.RectSize * this.RectCount
    };

    this.previous = null;
    this.adjust(this.RectCount, this.RectSize); // draw initial
    /** @this {Rects} */
    return this;
  },

  /**
   * Adjust the rectangle count and rectangle size drawn
   *
   * @param {number} count Count of the rectangles that are able to fit onto the grid
   * @param {number} RectSize Rectangle size in px
   */
  adjust: function (count, RectSize) {
    this.RectCount = count;
    this.RectSize = RectSize;
    this.Rects = createArray(this.RectCount, this.RectCount);
    if (this.drawn) {
      for (let i = 0; i < this.Rects.length; ++i) {
        for (let j = 0; j < this.Rects[i].length; ++j) {
          this.fillStyle = "rgba(250, 250, 250, 256)";
          this.fillShape(i * this.RectSize, j * this.RectSize);
        }
      }
    }
  },

  /**
   * Flattens the given numbers down to the specific starting coordinates of a rectangle
   *
   * @param {number} x X-Coordinate
   * @param {number} y Y-Coordinate
   * @returns {Coordinates} Starting point of Rectangle by the given coordinates
   */
  flatten: function (x, y) {
    return {
      x: Math.floor(x / this.RectSize) * this.RectSize,
      y: Math.floor(y / this.RectSize) * this.RectSize
    };
  },

  /**
   * Translates given coordinates to the actual array-index the supplied rectangle has
   *
   * @param {number} x X-Coordinate to translate
   * @param {number} y Y-Coordinate to translate
   * @returns {Indicies} The rectangles index  
   */
  translateToArray: function (x, y) {
    return {
      x: Math.floor(x / this.RectSize),
      y: Math.floor(y / this.RectSize)
    };
  },

  /**
   * Checks weither a rectangle is set or not currently
   *
   * @param {number} x X-Coordinate 
   * @param {number} y Y-Coordinate
   * @returns {boolean} True if set, false in case it isnt.
   */
  isRectSet: function (x, y) {
    let flat = flatten(x, y);
    if (!isset(this.Rects[y][x]) || this.Rects[y][x] == null) {
      return false;
    }
    return true;
  },

  /**
   * Sets a previous processed/visited rectangle
   *
   * @param {number} x X-Coordinate
   * @param {number} y Y-Coordinate
   */
  setPrevious: function (x, y) {
    let flat = this.flatten(x, y);
    this.previous = flat;
  },

  /**
   *
   *
   * @param {number} x
   * @param {number} y
   * @returns {boolean} If it is the same rectangle as before, returns true, otherwise false
   */
  isSameRect: function (x, y) {
    let flat = this.flatten(x, y);
    if (flat.x == this.previous.x && flat.y == this.previous.y) return true;
    else return false;
  },

  /**
   *
   */
  clear: function () {
    this.ctx.clearRect(0, 0, canvas.width, canvas.height);
    this.Rects = createArray(this.RectCount, this.RectCount);
  },

  /**
   * Compares `newcol` to `oldcolor` to check if `newcolor` is in specified range(`bound`).
   * Arguments are expected to be normal hex-rgb-values(e.g. #00FF00)
   * 
   * @param {string} oldcolor Old color to check against
   * @param {string} newcolor New color 
   * @param {number} bound Integer representing a range in which newcolor is allowed to be in range of oldcolor
   * @returns {boolean} When it is in specified range, returns true, otherwise false
   */
  inBetween: function (oldcolor, newcolor, bound) {
    let oldColInt = Math.floor(parseInt(oldcolor.substring(1), 16) / 10000);
    let NewColInt = Math.floor(parseInt(newcolor.substring(1), 16) / 10000);
    return between(NewColInt, oldColInt - bound, oldColInt + bound);
  },

  /**
   * Fill specific rectangle at coordinates
   *
   * @param {number} x X-Coordinate of the rectangle to fill
   * @param {number} y Y-Coordinate of the rectangle to fill
   */
  fillShape: function (x, y) {
    //floor it first so we get always the correct rectangle to work with
    let xArr = Math.floor(x / this.RectSize),
      yArr = Math.floor(y / this.RectSize);

    x = xArr * this.RectSize;
    y = yArr * this.RectSize;

    if (x > this.px - 1)
      return;
    if (y > this.px - 1)
      return;

    if (this.FloodFill) {
      let oldcolor = imgdataToRGB(
        this.ctx.getImageData(
          x + this.RectSize / 2,
          y + this.RectSize / 2,
          1, 1).data).toUpperCase();
      this.fill(x, y, oldcolor, this.fillStyle);
      return;
    }
    this.Rects[yArr][xArr] = new String(this.fillStyle);
    this.Rects[yArr][xArr] = this.fillStyle;
    this.ctx.fillStyle = this.fillStyle;
    this.ctx.fillRect(x, y, this.RectSize + 1, this.RectSize + 1);
  },

  /**
   * Flood-Filling algorithm, "stolen" from pseudocode, rewritten with a stack to be iterative, not recursive
   * reworked to fill rectangles instead of pixels
   *
   * @param {number} xcord
   * @param {number} ycord
   * @param {string} OldColor
   * @param {string} NewColor
   */
  fill: function (xcord, ycord, OldColor, NewColor) {
    let touched = createArray(this.RectCount + 1, this.RectCount + 1);
    let CoordStack = [];
    CoordStack.push({
      x: xcord,
      y: ycord
    });
    this.fillStyle = NewColor;
    this.FloodFill = false; // reset to false in case we've to draw which would yield us to a endless recursion

    while (CoordStack.length != 0) {
      var curCoords = CoordStack.pop();
      var SeenArray = this.translateToArray(curCoords.x, curCoords.y);

      // speeds up the whole process by skipping any invalid number that we might have introduced by not checking bounds when pushing new items as seen below
      if (
        curCoords.x < 0 || curCoords.y < 0 ||
        SeenArray.x > this.RectCount || SeenArray.y > this.RectCount
      ) {
        continue;
      }

      // skip already seen Rects
      if (touched[SeenArray.y][SeenArray.x] == true) continue;

      let flattenedCoords = this.flatten(curCoords.x, curCoords.y);

      // set
      let CurrentPixelColor = imgdataToRGB(
        this.ctx.getImageData(
          flattenedCoords.x + this.RectSize / 2,
          flattenedCoords.y + this.RectSize / 2, 1, 1).data).toUpperCase();

      touched[SeenArray.y][SeenArray.x] = true; // Mark Rect as seen
      if (CurrentPixelColor == OldColor || this.inBetween(OldColor, CurrentPixelColor, 5)) {

        // bounds check(we dont have to iterate over non-existent cols&rows)
        if ((SeenArray.x + 1) * this.RectSize > this.px + 2) {
          continue;
        }
        if ((SeenArray.y + 1) * this.RectSize > this.px + 2) {
          continue;
        }

        this.fillShape(curCoords.x, curCoords.y);

        //fill Stack with our new values - we dont care about validation, that's done later or earlier(depends
        // on your point of view)
        CoordStack.push({
          x: SeenArray.x * this.RectSize,
          y: (SeenArray.y + 1) * this.RectSize
        });
        CoordStack.push({
          x: SeenArray.x * this.RectSize,
          y: (SeenArray.y - 1) * this.RectSize
        });
        CoordStack.push({
          x: (SeenArray.x + 1) * this.RectSize,
          y: SeenArray.y * this.RectSize
        });
        CoordStack.push({
          x: (SeenArray.x - 1) * this.RectSize,
          y: SeenArray.y * this.RectSize
        });
      }
    }
    this.FloodFill = true;
    return;
  }
};

/**
 * Checks a given variable for being initialised and set
 *
 * @param {variable} variable
 * @returns {boolean} Returns true if set, false in case it isnt.
 */
function isset(variable) {
  return typeof variable !== "undefined";
}

/**
 * Create an multidimensional Array of the specified length/size
 *
 * @param {number...} length Elements the array should have
 * @returns {Array[][]} Initialized array of given size 
 */
function createArray(length) {
  let arr = new Array(length || 0),
    i = length;

  if (arguments.length > 1) {
    var args = Array.prototype.slice.call(arguments, 1);
    while (i--) arr[length - 1 - i] = createArray.apply(this, args);
  }

  return arr;
}

/**
 * Converts an uint8 array(as delivered by getimagedata) to an html-hex-color.
 *
 * @param {ImageData} imgdata ImageData of the pixel you want to read the color from
 * @returns {string} RGB from the specified data-field as a string-hex-value
 */
function imgdataToRGB(imgdata) {
  let returner = "#";
  for (let i = 0; i < imgdata.length - 1; ++i) {
    returner += imgdata[i].toString(16);
  }
  return returner;
}

/**
 * Checks a given value if it is in a specified range
 *
 * @param {number} x Value to check
 * @param {number} min Minimum size value should have
 * @param {number} max Maximum size value can have
 * @returns {boolean} False when not, true in case it is.
 */
function between(x, min, max) {
  return x >= min && x <= max;
}

/**
 * Debug Function, can be turned on by setting `DEBUGVAR` to `true`.
 * 
 * @param {string} message Message that gets fed to `console.log()`
 */
function DEBUG(message) {
  if (DEBUGVAR == true) console.log(message);
  else;
}
