import 'styles/index.scss';
import axios from 'axios';
import $ from 'jquery';
import 'styles/bootstrap.lux.min.css';
import 'svg.js';
import 'svg.draw.js';
import 'bootstrap';

import styles, { colors } from './styles';

import { polygonToString } from './helpers';


var hull = require('hull.js');
import api from './api';

window.jQuery = $;
window.$ = $;

const RESTART = "RESTART";
const TRANSPARENT = "TRANSPARENT";
const SEARCH = "SEARCH";

const addPolygonBtn = $('#add-polygon');
const addPointBtn = $('#add-point');
const searchRangeBtn = document.getElementById('search-range');
const searchNearest = document.getElementById('search-nearest');
const extraView = document.getElementById('extra-view');
const messageView = $('#messages');


const draw = new SVG('svg-area').size('100%', '100%');
let newPolygon;
let rangeRectangle;
let currentPoints = [];
let lastPolygonPoint;
let mapPolygons = {};
let currentRegions = [];
let lastPoint;
let kNearestPoint;
let mapRegions = {};
let polygons = [];
let regions = [];

let isDrawingPoint = false
let isDrawingPolygon = false;
let isDrawingKPoint = false;
let isDrawingRegion = false;

function getRandomColor() {
  var letters = "9ABCDEF".split('');
  var color = '#';
  for (var i = 0; i < 6; i++ ) {
      color += letters[Math.floor(Math.random() * letters.length)];
  }
  return color;
}

function drawLoggerBody(regions) {
  let htmlContent = '';
  for (let key in regions) {
    const region = regions[key];
    if ( ! region.isLeaf) {
      continue;
    }
    htmlContent += `<div class="select-region" style="background-color: ${region.color}" data-region="${region.id}" id="select-r${region.id}"><span>R${region.id}</span></div>`;
  }

  $('#logger-regions').html(htmlContent);
}

const newPolygonColors = {
  polygonColor: {
    fill: colors.primaryTransparent,
  },
  polygonStroke: styles.newPolygonStroke,
}

const kSearchPolygonColors = {
  polygonColor: {
    fill: colors.secondaryTransparent,
  },
  polygonStroke: styles.regionBorder,
}

const transparentColors = {
  polygonColor: {
    opacity: 0,
  },
  polygonStroke: styles.transparentStroke
}

function createRectangle() {
  if (rangeRectangle) {
    rangeRectangle.remove();
    rangeRectangle = undefined;
  }
  rangeRectangle = draw.rect().fill('#ffffff55').draw();
  rangeRectangle.on('drawstart', (e) => {
    isDrawingRegion = true;
  });
}

function loadDefaultTemplate() {
  extraView.innerHTML = defaultTemplate;
}

draw.dblclick(() => {
  if (currentPoints.length) {
    currentPoints.pop();
    api.post("", {
      polygon: currentPoints,
    }).then((d) => {
      getRegions(true);
      removeNewPolygon();
    });
  }
});

function removeRegion() {
  isDrawingRegion = false;
  if (rangeRectangle) {
    rangeRectangle.remove();
    rangeRectangle = undefined;
  }
}

function removeNewPoint() {
  isDrawingPoint = false;
  if (lastPoint) {
    lastPoint.remove();
  }
}

function removeKPoint() {
  isDrawingKPoint = false;
  if (kNearestPoint) {
    kNearestPoint.remove();
    kNearestPoint = undefined;
  }
}

function removeNewPolygon(stopDrawing) {
  if (stopDrawing) {
    isDrawingPolygon = false;
  }
  currentPoints = [];
  if (lastPolygonPoint) {
    lastPolygonPoint.remove();
    lastPolygonPoint = undefined;
  }
  if (newPolygon) {
    newPolygon.remove();
    newPolygon = undefined;
  }
}

draw.click((e) => {
  if (isDrawingPolygon) {
    const previuosPointsLenght = currentPoints.length;
    currentPoints.push([e.offsetX, e.offsetY]);
    currentPoints = hull(currentPoints, Infinity);
    if (!previuosPointsLenght) {
      newPolygon = draw.polygon().fill("none").stroke(styles.newPolygonStroke);
      lastPolygonPoint = draw.circle(10).move(e.offsetX - 5, e.offsetY - 5);
    } else {
      newPolygon.plot(currentPoints);
      lastPolygonPoint.animate(100).move(e.offsetX - 5, e.offsetY - 5);
    }
  } else if (isDrawingPoint) {
    if (!lastPoint) {
      lastPoint = draw.circle(10).stroke(styles.newPolygonStroke).move(e.offsetX - 5, e.offsetY - 5);
    } else {
      lastPoint.animate(100).move(e.offsetX - 5, e.offsetY - 5);
    }
    const point = [e.offsetX, e.offsetY];
    api.post("", {
      polygon: [point],
    }).then((d) => {
      getRegions(true);
    });
  } else if (isDrawingKPoint) {
    if ( ! kNearestPoint) {
      kNearestPoint = draw.circle(10).fill('white').move(e.offsetX - 5, e.offsetY - 5);
    } else {
      kNearestPoint.animate(100).move(e.offsetX - 5, e.offsetY - 5);
    }
    rePaintPolygons([], RESTART);
    const point = [e.offsetX, e.offsetY];
    const $k = $('#nearest-k');
    if ($k) {
      const k = $k.val();
      api.post("nearest", {
        polygon: [point], k
      }).then((d) => {
        rePaintPolygons(d.data.polygons, SEARCH);
      });
    }
  } else if (isDrawingRegion) {
    const attrs = rangeRectangle.attr();
    const polygon = [
      [attrs.x, attrs.y],
      [attrs.x + attrs.width, attrs.y + attrs.height],
    ];
    console.log(polygon, [e.offsetX, e.offsetY]);
    api.post("range", {
      polygon
    }).then(d => {
      rePaintPolygons(d.data.polygons, SEARCH);
      // isDrawingRegion = false;
    });
  }
});

function addMessage(message, type) {
  const alertView = $(alertTemplate(message, type));
  messageView.append(alertView);
  window.setTimeout(function () {
    alertView.fadeTo(300, 0).slideUp(300, function () {
        $(this).remove();
    });
  }, 2000);
  return alertView;
}

function drawRegions(regions) {
  for (let i = 0; i < currentRegions.length; i += 1) {
    currentRegions[i].text.remove();
    currentRegions[i].polygon.remove();
  }
  currentRegions = [];
  for (let i = 0; i < regions.length; i += 1) {
    const { id, polygon, color, isLeaf } = regions[i];
    const firstPoint = polygon[0];
    const textColor = isLeaf ? color : '#ffffffaa';
    const textLabel = isLeaf ? 'R' : 'P';
    // const posit
    const textSvg = draw.text(`${textLabel}${id}`).fill(textColor).font({ size: 16 });
    if (isLeaf) {
      textSvg.move(
        polygon[0][0] + 3, polygon[0][1] + 3
      );
    } else {
      textSvg.move(
        polygon[3][0] + 3, polygon[3][1] + 3
      );
    }
    let stroke = {
      width: 2,
      color,
    }
    if ( !isLeaf ) {
      stroke = {
        width: 1,
        color: '#ffffff88',
      }
    }
    const polygonSvg = draw.polygon(
      polygonToString(polygon)
    ).fill("none").stroke(stroke);
    if (!isLeaf) {
      polygonSvg.attr({'stroke-dasharray': [10, 10]});
    }
    mapRegions[id] = regions[i];
    mapRegions[id].color = color;
    mapRegions[id].polygons = [];
    mapRegions[id].svg = polygonSvg;
    mapRegions[id].text = textSvg;
    currentRegions.push({
      text: textSvg,
      polygon: polygonSvg,
    })
  }
  drawLoggerBody(mapRegions);
}

function drawPolygons(polygons) {
  const polys = Object.keys(mapPolygons);
  for (let i = 0; i < polys.length; i += 1) {
    const key = polys[i];
    if (mapPolygons[key].svg) {
      mapPolygons[key].svg.remove();
    }
    if (mapPolygons[key].text) {
      mapPolygons[key].text.remove();
    }
  }
  for (let i = 0; i < polygons.length; i += 1) {
    const region = mapRegions[polygons[i].region];
    let svgPolygon, text;
    let point = point = polygons[i].polygon[0];
    if (polygons[i].polygon.length > 1) {
      svgPolygon = draw.polygon(
        polygonToString(polygons[i].polygon)
      ).fill(region.color + "AA").stroke({
        color: region.color,
        width: 2,
      });
    } else if (polygons[i].polygon.length === 1) {
      svgPolygon = draw.circle(12).fill(
        region.color
      ).stroke({
        width: 2, color: region.color, opacity: 1,
      }).move(point[0] - 6, point[1] - 6);
    }
    text = draw.text(`${polygons[i].id}`).move(
        point[0] + 6, point[1] + 7
      ).fill(region.color).font({ size: 10 });
    mapPolygons[polygons[i].id] = {
      svg: svgPolygon,
      text,
    };
    mapRegions[polygons[i].region].polygons.push(polygons[i].id);
  }
}

function rePaintPolygons(ids, mode=RESTART) {
  if (mode === RESTART) {
    drawPolygons(polygons);
    return;
  }

  for(let i = 0; i < ids.length; ++i) {
    let styles;
    if (mode === SEARCH) {
      styles = kSearchPolygonColors;
    }
    if (styles !== undefined) {
      mapPolygons[ids[i]].svg.attr(styles.polygonColor).stroke(styles.polygonStroke);
    }
  }
}


const defaultTemplate = `
<p>Select an option from menu</p>
`;

const addPolygon = `
<h5>Add Polygon</h5>
<p>Click to add a point. Double click to finish the drawing.</p>
<button id="cancel-drawing" class="btn btn-danger">Cancel</button>
`;

const addPoint = `
<h5>Add Polygon</h5>
<p>Click to add a point.</p>
<button id="cancel-drawing" class="btn btn-danger">Cancel</button>
`

const kNearestTemplate = `
<p>Click to draw a point an then select a valid k</p>
<form>
  <div class="form-group">
    <label for="nearest-k">K</label>
    <input id="nearest-k" type="number" value="2" min=1 />
  </div>
  <button id="cancel-drawing" class="btn btn-danger">Finish</button>
</form>
`;


const rangeSearchTemplate = `
<p>Click in any point to draw a rectangle</p>
`;

const rangeSearch = `<p></p>`

const alertTemplate = (message, type) => (`
<div class="alert alert-${type} alert-dismissible fade show" role="alert">
  ${message}
  <button type="button" class="close" data-dismiss="alert" aria-label="Close">
    <span aria-hidden="true">&times;</span>
  </button>
</div>`);


addPolygonBtn.click(() => {
  cancelDrawing();
  extraView.innerHTML = addPolygon;
  isDrawingPolygon = true;
});

addPointBtn.click(() => {
  cancelDrawing();
  extraView.innerHTML = addPoint;
  isDrawingPoint = true;
})

searchRangeBtn.addEventListener('click', () => {
  cancelDrawing();
  extraView.innerHTML = rangeSearchTemplate;
  createRectangle();
});

searchNearest.addEventListener('click', () => {
  cancelDrawing();
  extraView.innerHTML = kNearestTemplate;
  isDrawingKPoint = true;
});

function cancelDrawing() {
  removeNewPolygon(true);
  removeNewPoint();
  removeKPoint();
  rePaintPolygons([], RESTART);
  removeRegion();
}

$('.information').on('click', '#cancel-drawing', () => {
  cancelDrawing();
  loadDefaultTemplate();
  $('.nav-link').removeClass('active');
});


$('.nav-link').click(function() {
  $('.nav-link').removeClass('active');
  $(this).addClass('active');
});

$('#logger-regions').on('click', '.select-region', (e) => {
  const currentTarget = $(e.currentTarget);
  const regionId = currentTarget.data('region');
  const isActive = currentTarget.hasClass('active');
  currentTarget.toggleClass('active');
  mapRegions[regionId].svg.attr({ opacity: isActive ? 1 : 0 });
  mapRegions[regionId].text.attr({ opacity: isActive ? 1 : 0 });
  for (let i = 0; i < mapRegions[regionId].polygons.length; i += 1){
    const polygonId = mapRegions[regionId].polygons[i];
    mapPolygons[polygonId].svg.attr({ opacity: isActive ? 1 : 0 });
    mapPolygons[polygonId].text.attr({ opacity: isActive ? 1 : 0 });
  }
});

$('#reset-button').on('click', () => {
  console.log("muajaja");
});

function getRegions(shouldDrawPolygons) {
  api.get().then((d) => {
    polygons = d.data.polygons;
    regions = d.data.regions;
    if (polygons.length) {
      drawRegions(regions);
      if (shouldDrawPolygons) {
        drawPolygons(polygons);
      }
    } else {
      $('#treeModal').modal('show');
    }
  });
}

getRegions(true);

