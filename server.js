var height = 16;
var numBoards = 3;

var express = require('express.io');
var canvas = require('canvas');
var board = require('rpi-rgb-led-matrix');

app = express();
app.http().io();

app.use(function(req, res, next) {
  res.sendJSON = function(obj) {
    res.set({'Content-Type': 'application/json'});
    res.status(200);
    res.send(obj);
  };
  next();
});

app.get('/', function(req, res) {
  res.status(200);
  res.sendfile(__dirname + "/index.html");
})

app.post('/api/v1/fill', function(req, res) {
  board.fill(255, 255, 255);
  res.sendJSON('ok');
})

board.start(height, numBoards);

app.listen(80);
