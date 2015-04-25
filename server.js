var width = 96;
var height = 16;
var numBoards = 3;

var express = require('express.io');
var bodyParser = require('body-parser');
var canvas = require('canvas');
var board = require('rpi-rgb-led-matrix');

var myStateQueue = new stateQueue();

function state(namespace, type, duration, id) {
  this.stateID = id;
  this.namespace = namespace;
  this.type = type;
  this.duration = duration;
}

function stateQueue() {
  this.states = [];
}

stateQueue.prototype.insert = function (state, index) {
  if (index == undefined) index = this.states.length;
  console.log(this.states);
  if (index <= this.states.length && index >= 0) {
    this.states.splice(index, 0, state);
  }
  console.log(this.states);
}

stateQueue.prototype.removeNamespace = function (namespace) {
  this.states.forEach( function (theState, theIndex, theArray) {
    if (theState.namespace == namespace) {
      theArray.splice(theIndex, 1);
    }
    console.log(this.states);
  });
}

app = express();
app.http().io();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
  extended: true
}));

app.use(function(req, res, next) {
  res.sendJSON = function(obj, status) {
    if (status === undefined) status = 200;
    res.set({'Content-Type': 'application/json'});
    res.status(status);
    res.send(obj);
  }
  next();
});

function stringCanvas(displayString) {
  var str = 'Senior Design';
  var stringWidth = str.length * 15;
  var stringCanvas = new canvas(stringWidth, height);
  var ctx = stringCanvas.getContext('2d');
  ctx.font = "22px monospace";

  var shift = 4;
  var offset = 0;

  setInterval(function() {

    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, stringWidth, 16);

    ctx.fillStyle = "#00FF79";
    ctx.fillText(str, 0, height);

    board.drawCanvas(ctx, width, height);
    if (offset > stringWidth) {
      ctx.translate(offset+width, 0);
      offset = -width;
    }
    else
      ctx.translate(-shift, 0);

    offset += shift;

  }, 25)
}

app.get('/', function(req, res) {
  res.status(200);
  res.sendfile(__dirname + "/index.html");
});

app.post('/api/v1/fill', function(req, res) {
  console.log(req.body)
  if (req.body.color)
    board.fill(req.body.color[0], req.body.color[1], req.body.color[2]);
  else
    board.fill(255, 255, 255);
  res.sendJSON('ok');
});

app.post('/api/v1/insert', function(req, res) {
  console.log(req.body)
  if (req.body.state == "text") {
    console.log(JSON.stringify(req.body));
    myStateQueue.insert(new state(req.body.namespace, req.body.type, req.body.duration, req.body.id));
    stringCanvas(req.body.text);
    res.sendJSON('ok');
  }
  else
    res.sendJSON('JSON Error', 400);
});

board.start(height, numBoards);

var testStateQueue = new stateQueue();
myStateQueue.insert(new state("permanent", "text", "500", "1"));
console.log(JSON.stringify(myStateQueue.states[0]));

app.listen(80);
