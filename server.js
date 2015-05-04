var width = 96,
    height = 16,
    numBoards = 3,
    defaultDuration = 5000,
    dateCanvas = undefined,
    timeCanvas = undefined;

var express = require('express.io'),
    app = express(),
    bodyParser = require('body-parser'),
    Canvas = require('canvas'),
    Font = Canvas.Font,
    path = require('path'),
    board = require('rpi-rgb-led-matrix'),
    moment = require('moment');

var myStateQueue = new stateQueue();

var font = new Font('PixelMix', path.join(__dirname, 'fonts', 'pixelmix', 'pixelmix.ttf'));

function renderText(text, color) {
  if (color === undefined) color = 'red';
  var canvas = new Canvas(1, 8);
  var ctx = canvas.getContext('2d');
  ctx.font = '8px PixelMix';
  canvas.width = ctx.measureText(text).width;
  ctx.fillStyle = color;
  ctx.fillText(text, 0, 8);
  return canvas;
}

function renderDateCanvas() {
  var now = moment();
  dateCanvas = renderText(now.format('MMM DD'));
  setTimeout(renderDateCanvas, moment().add(1, 'day').startOf('day') - moment() + 100);
}

function renderTimeCanvas() {
  var now = moment();
  timeCanvas = renderText(now.format('HH:mm'));
  setTimeout(renderTimeCanvas, moment().add(1, 'minute').startOf('minute') - moment() + 100);
}

function state(obj) {
  obj.stateID = (function generateUIDNotMoreThan1million() {
    return ("0000" + (Math.random()*Math.pow(36,4) << 0).toString(36)).slice(-4); })();
  if (obj.namespace === undefined) throw new Error("Missing namespace property");
  if (obj.type === undefined) throw new Error("Missing type property");
  else if (obj.type == 'text') return new textstate(obj);
  else if (obj.type == 'png') return new pngstate(obj);
}

function textstate(obj) {
  if (obj.text === undefined) throw new Error("Mising text property");
  this.namespace = obj.namespace;
  this.type = obj.type;
  this.stateID = obj.stateID;
  this.text = obj.text;
  this.color = obj.color === undefined ? 'red' : obj.color;

  /* Render the text to a canvas */
  this._textCanvas = renderText(this.text, this.color);
  this._scrolling = this._textCanvas.width > width;
  if (this._scrolling)
    this.duration = 2000 + (50 * (this._textCanvas.width - width + 32));
  else
    this.duration = defaultDuration;
}

textstate.prototype.render = function () {
  var self = this;
  self._offset = 2;

  self._tickTimeout = undefined;

  function tick() {
    clearTimeout(self._tickTimeout);
    if (self._scrolling) {
      self._offset -= 2;
      if (self._offset == 0)
        // pause for 2 seconds so the beginning of the text can be read
        self._tickTimeout = setTimeout(tick, 2000);
      else
        self._tickTimeout = setTimeout(tick, 100);
    }

    var canvas = new Canvas(width, height);
    var ctx = canvas.getContext('2d');
    ctx.drawImage(dateCanvas, 0, 0);
    ctx.drawImage(timeCanvas, width - timeCanvas.width, 0);

    if (self._scrolling) {
      ctx.drawImage(self._textCanvas, self._offset, 8);
    } else {
      ctx.drawImage(self._textCanvas, Math.floor((width - self._textCanvas.width) / 2), 8);
    }

    board.drawCanvas(ctx, width, height);
  }

  tick();
}

textstate.prototype.finish = function () {
  clearTimeout(this._tickTimeout);
}

function pngstate(obj) {
  if (obj.data === undefined) throw new Error("Mising data property");
  this.namespace = obj.namespace;
  this.type = obj.type;
  this.stateID = obj.id;
  this.data = obj.data;
  if (obj.duration)
    this.duration = obj.duration;
  else
    this.duration = defaultDuration;
}

function stateQueue() {
  this.states = [];
  this.currentState = 0;
  this.running = false;
}

stateQueue.prototype.insert = function (reqState, index) {
  if (index === undefined) index = this.states.length;
  if (index <= this.states.length && index >= 0) {
    this.states.splice(index, 0, reqState);
  }
  if (index <= this.currentState)
    this.currentState++;
  return reqState.stateID;
}

stateQueue.prototype.removeNamespace = function (namespace) {
  var i = this.states.length;
  while(i--) {
    if (this.states[i].namespace == namespace) {
      this.states.splice(i, 1);
      if (i <= this.currentState)
        this.currentState--
    }
  }
}

stateQueue.prototype.removeID = function (id) {
  var i = this.states.length;
  while(i--) {
    if (this.states[i].stateID == id) {
      this.states.splice(i, 1);
      if (i <= this.currentState)
        this.currentState--;
    }
  }
}

stateQueue.prototype.nextState = function() {
  var self = this;

  if (this.states.length > 0) {
    this.running = true;
    var next = this.currentState + 1;
    if (next >= this.states.length)
      next = 0;

    if (this.currentState != next) {
      this.currentState = next;
    }

    this.states[this.currentState].render();

    var curState = this.states[this.currentState];
    this._nextStateTimeout = setTimeout(function () {
      if (curState.finish)
        curState.finish();
      self.nextState();
    }, this.states[this.currentState].duration);
  } else {
    board.fill(0, 0, 0);
    this.running = false;
  }
}

stateQueue.prototype.print = function() {
  this.states.forEach(function (theState, theIndex, theArray) {
    console.log(theState);
  });
}

app.http().io();

app.use(express.static('public'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
  extended: true
}));

app.use(function(err, req, res, next) {
  if(err.status == 400) {
    console.error(err.stack);
    res.status(400).sendJSON({
      'status': 'error',
      'error_msg': err.stack,
    });
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

app.get('/', function(req, res) {
  res.render('index', {});
});

app.post('/api/v1/fill', function(req, res) {
  if (req.body.color)
    board.fill(req.body.color[0], req.body.color[1], req.body.color[2]);
  else
    board.fill(255, 255, 255);
  res.sendJSON({'status': 'ok'});
});

app.post('/api/v1/insert', function(req, res) {
  var id = myStateQueue.insert(new state(req.body));
  // myStateQueue.print();
  if (!myStateQueue.running) {
    myStateQueue.nextState();
  }
  res.sendJSON({'status': 'ok', 'stateID': id});
});

app.post('/api/v1/removenamespace', function(req, res) {
  myStateQueue.removeNamespace(req.body.namespace);
  res.sendJSON({'status': 'ok'});
});

app.post('/api/v1/removeid', function(req, res) {
  myStateQueue.removeID(req.body.stateID);
  res.sendJSON({'status': 'ok'});
});

board.start(height, numBoards);

renderDateCanvas();
renderTimeCanvas();

console.log("ready");

app.listen(80);
