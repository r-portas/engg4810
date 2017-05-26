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

let serialDevice = null;

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

  // Logging
  socket.on('start-log', (data) => {
    const line = `s ${data.sampleRate} ${data.sampleNum}`;
    console.log(`Sending ${line} over serial`);
    if (serialDevice != null) {
      if (serialDevice.isOpen()) {
        serialDevice.write(`${line}\n`);
      }
    }
  });

  /**
   * A range is changing the measurement mode
   */
  socket.on('sampling-rate', (sampleRate) => {
    console.log(`Sending '${sampleRate}' over serial`);
    if (serialDevice != null) {
      if (serialDevice.isOpen()) {
        serialDevice.write(`${sampleRate}\n`);
      }
    }
  });

  /**
   * Set the brightness of the LCD display
   */
  socket.on('set-brightness', (brightness) => {
    console.log(`Sending '${brightness}' over serial`);
    if (serialDevice != null) {
      if (serialDevice.isOpen()) {
        serialDevice.write(`${brightness}\n`);
      }
    }
  });

  /**
   * A range is changing the measurement mode
   */
  socket.on('set-range', (newRange) => {
    console.log(`Sending '${newRange}' over serial`);
    if (serialDevice != null) {
      if (serialDevice.isOpen()) {
        serialDevice.write(`${newRange}\n`);
      }
    }
  });

  socket.on('setport', (newport) => {
    console.log(`Connecting to ${newport.comName}`);

    // Connect to the device
    serialDevice = new SerialPort(newport.comName,
      { parser: SerialPort.parsers.readline('\n') },
      (err) => {
        if (err) {
          console.log(`Error: ${err.message}`);
        }

        // Notify the clients that the server connected
        socket.emit('deviceconnected', newport.comName);
      });

    serialDevice.on('data', (data) => {
      const trimmed = data.trim();
      console.log(`Receiving '${trimmed}' over serial`);
      socket.emit('devicedata', trimmed);
    });

    // fl is logging-finished
    serialDevice.on('lf', () => {
      socket.emit('logging-finished');
    });

    serialDevice.on('close', () => {
      socket.emit('devicedisconnected');
    });

    /**
     * Disconnect all serial communications
     */
    socket.on('device-disconnect', () => {
      if (serialDevice != null) {
        if (serialDevice.isOpen()) {
          serialDevice.close();
        }
      }
    });
  });
});

http.listen(port, () => console.log(`Multimeter running on port ${port}`));

module.exports = app;
