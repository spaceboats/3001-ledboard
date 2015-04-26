var width = 96,
    height = 16,
    numBoards = 3,
    defaultDuration = 5000;

var app = require('express.io')(),
    bodyParser = require('body-parser'),
    swig = require('swig'),
    canvas = require('canvas'),
    board = require('rpi-rgb-led-matrix');

app.engine('html', swig.renderFile);
app.set('view engine', 'html');
app.set('views', __dirname + '/views');


var myStateQueue = new stateQueue();

var changeStateInterval = setTimeout(myStateQueue.nextState, defaultDuration);

function state(namespace, type, duration, id) {
  this.namespace = namespace;
  this.type = type;
  this.duration = duration;
  this.stateID = id;
}

function stateQueue() {
  this.states = [];
  this.currentState = 0;
  this.running = false;
}

stateQueue.prototype.insert = function (state, index) {
  if (index == undefined) index = this.states.length;
  console.log(this.states);
  if (index <= this.states.length && index >= 0) {
    this.states.splice(index, 0, state);
  }
  if (index <= this.currentState)
    this.currentState++;
}

stateQueue.prototype.removeNamespace = function (namespace) {
  this.states.forEach(function (theState, theIndex, theArray) {
    if (theState.namespace == namespace) {
      theArray.splice(theIndex, 1);
      if (theIndex <= this.currentState)
        this.currentState--
    }
  });
}

stateQueue.prototype.removeID = function (id) {
  this.states.forEach(function (theState, theIndex, theArray) {
    if (theState.id == id) {
      theArray.splice(theIndex, 1);
      if (theIndex <= this.currentState)
        this.currentState--
    }
  });
}

stateQueue.prototype.nextState = function() {
  if (this.states.length > 0) {
    if (this.currentState > this.states.length)
      this.currentState = 0;
    this.running = true;

    if (this.states[currentState].type == "text") {
      stringCanvas(this.states[currentState].text);
    }

    interval = setTimeout(stateQueue.nextState, states[currentState].duration);
    this.currentState++;
  }
}

stateQueue.prototype.print = function() {
  this.states.forEach(function (theState, theIndex, theArray) {
    console.log(theState);
  });
}

app.http().io();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
  extended: true
}));

app.use(function(err, req, res, next) {
  if(err.status == 400) {
    console.error(err.stack);
    res.status(400).send('bad request');
  }
});

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
  var stringWidth = displayString.length * 15;
  var stringCanvas = new canvas(stringWidth, height);
  var ctx = stringCanvas.getContext('2d');
  ctx.font = "22px monospace";

  var shift = 4;
  var offset = 0;

  setInterval(function() {

    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, stringWidth, 16);

    ctx.fillStyle = "#00FF79";
    ctx.fillText(displayString, 0, height);

    board.drawCanvas(ctx, width, height);
    if (offset > stringWidth) {
      ctx.translate(offset+width, 0);
      offset = -width;
    }
    else
    ctx.translate(-shift, 0);

  offset += shift;

  }, 25);
}

app.get('/', function(req, res) {
  res.render('index', {});
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
  if (req.body.type == "text") {
    console.log(JSON.stringify(req.body));
    myStateQueue.insert(req.body)
    res.sendJSON('ok');
  }
  else
    res.sendJSON('JSON Error', 400);
});

app.post('/api/v1/removenamespace', function(req, res) {
  myStateQueue.removeNamespace(req.body.namespace);
  res.sendJSON('ok');
});

app.post('/api/v1/removeid', function(req, res) {
  myStateQueue.removeID(req.body.id);
  res.sendJSON('ok');
});

board.start(height, numBoards);

app.listen(80);
