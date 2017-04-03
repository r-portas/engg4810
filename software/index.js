/**
 * The main entry point of the application
 *
 * @author Roy Portas <royportas@gmail.com>
 */

const express = require('express');
const SerialPort = require('serialport');

// Setup socketio server
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

const port = process.env.PORT || 3000;

// Host files in the public folder
app.use(express.static('public'));

io.on('connection', (socket) => {
  console.log('user connected');

  socket.on('disconnect', () => {
    console.log('User disconnected');
  });

  /**
   * Returns a list of connected serial ports
   */
  socket.on('getports', () => {
    SerialPort.list((err, ports) => {
      if (err) {
        console.log(err);
        return;
      }

      socket.emit('portslist', ports);
    });
  });
});

http.listen(port, () => console.log(`Multimeter running on port ${port}`));
