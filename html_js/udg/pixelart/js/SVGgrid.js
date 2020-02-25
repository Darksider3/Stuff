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

/**
 * This class is actually the view-manager and controller, in one, 
 * for the SVG in the background shown on the canvas
 *
 * @param {ElementContentEditable} ele Element to draw SVG on
 * @param {number} [count=8]  Rectangle count on SVG
 * @param {number} [size=400] whole picture size in pixels
 * @returns
 */
const SVGGrid = {
  init: function (ele, count = 8, size = 400) {
    this.element = ele;
    this.svgEle = false;
    this.RectCount = count;
    this.size = size;
    this.activated = false;
    this.gridSVGs = {
      8: {
        img: "svg/8x8.svg",
        scale: 3
      },
      16: {
        img: "svg/16x16.svg",
        scale: 3
      },
      32: {
        img: "svg/32x32.svg",
        scale: 3
      }
    };
    return this;
  },

  /**
   * Adjust count, size and (not implemented yet) scale of the SVG and, if already drawn, redraw it
   *
   * @param {number} count New rectangle count
   * @param {number} size New size in pixels of the element we draw our svg on
   */
  adjust: function (count, size, scale = 3) {
    this.RectCount = count;
    this.size = size;
    if (this.activated) {
      this.draw();
    }
  },

  /**
   * Draws the SVG in the background of said element
   *
   */
  draw: function () {
    this.activated = true;
    this.element.style.background =
      "url(" + this.gridSVGs[this.RectCount].img + ")";
    this.element.style.backgroundSize = "cover";
  },

  /**
   * Toggles the svg on and off
   *
   * @param {ElementContentEditable} element HTML element that acts as a toggle switch
   */
  toggle: function (element) {
    if (this.activated) {
      element.textContent = "Show grid!"
      this.hide();
    }
    else {
      element.textContent = "Hide grid!";
      this.draw();
    }
  },

  /**
   * Hide the SVG grid
   *
   */
  hide: function () {
    this.activated = false;
    this.element.style.background = "";
  }
};
