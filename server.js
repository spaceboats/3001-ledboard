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

board.start(height, numBoards);

app.listen(80);
