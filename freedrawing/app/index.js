import 'styles/index.scss';
import axios from 'axios';
import $ from 'jquery';
import 'styles/bootstrap.lux.min.css';
import 'svg.js';
import 'svg.draw.js';
import 'bootstrap';

import styles, { colors } from './styles';

import { regions, polygons } from './data';
import { polygonToString } from './helpers';


var hull = require('hull.js');
import api from './api';

window.jQuery = $;
window.$ = $;


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

let isDrawingPoint = false
let isDrawingPolygon = false;
let isDrawingKPoint = false;

function cleanPolygons() {
  if (rangeRectangle) {
    rangeRectangle.remove();
  }
}

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
    htmlContent += `<div class="select-region" data-region="${region.id}" id="select-r${region.id}"><span>R${region.id}</span> <span style="background-color: ${region.color}; width: 15px; height: 15px; position: absolute;"></span></div>`;
  }

  $('#logger-body').html(htmlContent);
}

const newPolygonColors = {
  polygonColor: {
    fill: colors.primaryTransparent,
  },
  polygonStroke: styles.newPolygonStroke,
}

const kSearchPolygonColors = {
  polygonColor: {
    fill: colors.successTransparent,
  },
  polygonStroke: styles.kPointStroke,
}

const transparentColors = {
  polygonColor: {
    opacity: 0,
  },
  polygonStroke: styles.transparentStroke
}

function createRectangle() {
  cleanPolygons();
  rangeRectangle = draw.rect().fill('#044B9466').draw();
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
      const newPolygon = {
        id: d.data.id,
        polygon: currentPoints,
      };
      drawPolygons([newPolygon]);
      addMessage('Polygon added successfully.', 'success');
      getRegions(false);
      removeNewPolygon();
    });
  }
});

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
  }
}

function removeNewPolygon() {
  // isDrawingPolygon = false;
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
  } else if(isDrawingPoint) {
    if (!lastPoint) {
      lastPoint = draw.circle(10).stroke(styles.newPolygonStroke).move(e.offsetX - 5, e.offsetY - 5);
    } else {
      lastPoint.animate(100).move(e.offsetX - 5, e.offsetY - 5);
    }
    const point = [e.offsetX, e.offsetY];
    api.post("", {
      polygon: [point],
    }).then((d) => {
      getRegions();
      const newPolygon = {
        id: d.data.id,
        polygon: [point],
      };
      polygons.push(newPolygon);
      drawPolygons([newPolygon]);
    });
  } else if(isDrawingKPoint) {
    if ( ! kNearestPoint) {
      kNearestPoint = draw.circle(10).fill('green').stroke(styles.newPolygonStroke).move(e.offsetX - 5, e.offsetY - 5);
    } else {
      kNearestPoint.animate(100).move(e.offsetX - 5, e.offsetY - 5);
    }
    rePaintPolygons(Object.keys(mapPolygons), newPolygonColors);
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
    const color = getRandomColor();
    const { id, polygon } = regions[i];
    const firstPoint = polygon[0];
    const textSvg = draw.text(`R${id}`).move(
      firstPoint[0] + 3, firstPoint[1] + 3
    ).fill(color).font({ size: 18 });
    const polygonSvg = draw.polygon(
      polygonToString(polygon)
    ).fill("none").stroke({
      width: 1, color
    });
    mapRegions[id] = regions[i];
    mapRegions[id].color = color;
    mapRegions[id].polygons = [];
    mapRegions[id].svg = polygonSvg;
    currentRegions.push({
      text: textSvg,
      polygon: polygonSvg,
    })
  }
  drawLoggerBody(mapRegions);
}

function drawPolygons(polygons, styles=newPolygonColors) {
  for (let i = 0; i < polygons.length; i += 1) {
    const region = mapRegions[polygons[i].region];
    let svgPolygon;
    if (polygons[i].polygon.length > 1) {
      svgPolygon = draw.polygon(
        polygonToString(polygons[i].polygon)
      ).fill(region.color).stroke(styles.polygonStroke);
    } else if (polygons[i].polygon.length === 1) {
      const point = polygons[i].polygon[0];
      svgPolygon = draw.circle(12).fill(
        region.color
      ).stroke({
        width: 2, color: region.color, opacity: 1,
      }).move(point[0] - 6, point[1] - 6);
    }
    mapPolygons[polygons[i].id] = svgPolygon;
    mapRegions[polygons[i].region].polygons.push(polygons[i].id);
  }
}

function rePaintPolygons(ids, styles=kSearchPolygonColors) {
  for(let i = 0; i < ids.length; ++i) {
    mapPolygons[ids[i]].attr(styles.polygonColor).stroke(styles.polygonStroke);
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
    <input id="nearest-k" type="number" value="2" />
  </div>
  <button id="cancel-drawing" class="btn btn-danger">Cancel</button>
  <button id="submit-k-nearest" class="btn btn-primary">Submit</button>
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
  isDrawingPolygon = true;
  extraView.innerHTML = addPolygon;
  // newPolygon = ;
});

addPointBtn.click(() => {
  extraView.innerHTML = addPoint;
  isDrawingPoint = true;
})

searchRangeBtn.addEventListener('click', () => {
  extraView.innerHTML = rangeSearchTemplate;
  createRectangle();
});

searchNearest.addEventListener('click', () => {
  extraView.innerHTML = kNearestTemplate;
  isDrawingKPoint = true;
});

$('.information').on('click', '#cancel-drawing', () => {
  removeNewPolygon();
  removeNewPoint();
  removeKPoint();
  loadDefaultTemplate();
  $('.nav-link').removeClass('active');
});

$('.information').on('click', '#submit-k-nearest', () => {
  const k = $('#nearest-k').val();
  const point = [kNearestPoint.x(), kNearestPoint.y()];
  rePaintPolygons(Object.keys(mapPolygons), newPolygonColors);
  api.post("nearest", {
    polygon: [point], k
  }).then((d) => {
    rePaintPolygons(d.data.polygons);
  });
})
$('.nav-link').click(function() {
  $('.nav-link').removeClass('active');
  $(this).addClass('active');
});

$('#logger-body').on('click', '.select-region', (e) => {
  const regionId = $(e.currentTarget).data('region');
  rePaintPolygons(mapRegions[regionId].polygons, transparentColors);
  mapRegions[regionId].svg.attr({
    opacity: 0
  });
  // console.log('map', mapRegions[regionId]);
  // mapRegions[regionId]
})


function getRegions(shouldDrawPolygons) {
  api.get().then((d) => {
    console.log('d.data', d.data);
    drawRegions(d.data.regions);
    if (shouldDrawPolygons) {
      drawPolygons(d.data.polygons);
    }
  });
}

getRegions(true);

// draw.text("R1").move(2,2).fill("red");
// drawPolygons(polygons);

// const a = [[512, 56], [311, 268], [625, 238]];
// console.log('Convex', hull(a));


// api.get('/rtree').then(d => {
//   console.log('d', d);
// });

