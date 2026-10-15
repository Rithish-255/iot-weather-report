const express = require('express');
const http = require('http');
const cors = require('cors');
const bodyParser = require('body-parser');
const { Server } = require('socket.io');

const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(express.static('public'));

const server = http.createServer(app);
const io = new Server(server);

io.on('connection', (socket) => {
  console.log('Web client connected:', socket.id);
  socket.on('disconnect', () => {
    console.log('Client disconnected:', socket.id);
  });
});

app.post('/data', (req, res) => {
  const data = req.body;
  console.log('[POST /data] Received:', data);
  data.server_ts = new Date().toISOString();
  io.emit('sensor-data', data);
  res.json({ status: 'ok' });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Server listening on port ${PORT}`);
});
