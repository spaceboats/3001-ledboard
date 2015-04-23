var width = 96;
var height = 16;
var numBoards = 3;

var express = require('express.io');
var bodyParser = require('body-parser');
var canvas = require('canvas');
var board = require('rpi-rgb-led-matrix');

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
  var stringCanvas = new canvas(width, height);
  var ctx = stringCanvas.getContext('2d');
  ctx.font = "22px courier";

  setInterval(function() {
    var str = 'Senior Design';

    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, width, 16);

    ctx.fillStyle = "#00FF79";
    ctx.fillText(str, 0, height);

    board.drawCanvas(ctx, width, height);

  }, 1000)
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
  if (req.body.scene == "string") {
    if (req.body.string) {
      stringCanvas(req.body.string);
      res.sendJSON('ok');
    }
  }
  else
    res.sendJSON('JSON Error', 400);
});

board.start(height, numBoards);

app.listen(80);
