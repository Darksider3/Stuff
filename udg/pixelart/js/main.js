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


"use strict";
let dragged = false,
  canvasElem = document.querySelector("canvas"),
  context = canvas.getContext("2d"),
  toggleButton = document.querySelector('gridtoggle'),
  SVG = SVGGrid.init(canvasElem),
  Rec = Rects.init(context),
  CONFIG ={customColor:  false};
// set devicePixelRatio to a HiDi-specific value(better colors on good screens)
window.devicePixelRatio = 2.5;

// Turn off smoothing
context.imageSmoothingEnabled = false;
context.mozImageSmoothingEnabled = false;
context.oImageSmoothingEnabled = false;
context.webkitImageSmoothingEnabled = false;
context.msImageSmoothingEnabled = false;

// Finally, draw.
SVG.draw();

// HTML specific function
