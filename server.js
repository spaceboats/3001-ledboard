var express = require('express.io');
app = express();
app.http().io();

app.get('/', function(req, res){
  res.status(200);
  res.sendfile(__dirname + "/index.html");
})

app.listen(80);
/*
http.createServer(function(req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello World\n');
}).listen(80);
console.log('Server running');
*/
