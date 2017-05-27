/* global d3, moment, io, window, Vue */

/**
 * The main vue instance
 *
 * @author Roy Portas <royportas@gmail.com>
 */

new Vue({
  el: '#app',

  data() {
    return {
      // The main communication bus
      bus: {},
      date: {},

      socket: null,

      // Keeps a reference to the random data interval timer
      intervalRef: null,

      // True if we are connected to the multimeter
      isConnected: false,

      // True if the logging is paused
      isPaused: false,

      // The name of the device
      deviceName: '',


      data: [],

      // The current mode
      currentMode: '',

      // Tab settings
      currentTab: 'Chart',
      tabs: {
        CHART: 'Chart',
        DATA: 'Data',
        SETTINGS: 'Settings',
      },
    };
  },

  methods: {
    /**
     * Shows a notification through the snackbar
     */
    showSnackbar(message) {
      const sb = this.$refs.snackbar;
      const data = {
        message,
      };
      sb.MaterialSnackbar.showSnackbar(data);
    },

    /**
     * Add data entry
     */
    addEntry(date, value) {
      // Only add data if we aren't paused
      const isoString = date.toISOString();
      const parseTime = d3.isoParse;
      this.data.push({ date: parseTime(isoString), value, currentMode: this.currentMode });
    },

    /**
     * Checks if the current tab is active
     */
    checkTabActive(tabName) {
      return tabName === this.currentTab;
    },

    /**
     * Sets data, used for importing
     */
    setData(data) {
      for (let item of data) {
        const date = moment(item.date);
        this.addEntry(date, item.value);
      }
    },

    /**
     * Start random data feed
     */
    startRandomData() {
      this.intervalRef = setInterval(() => {
        this.date.add(1, 'hours');
        const rand = Math.floor((Math.random() * 40) + 1) - 20;
        this.addEntry(this.date, rand);
      }, 1000);
    },

    /**
     * Stop random data feed
     */
    stopRandomData() {
      clearInterval(this.intervalRef);
    },

    /**
     * Sets the program into a connected state
     */
    setConnected(deviceName) {
      this.deviceName = deviceName;
      this.isConnected = true;
      this.showSnackbar(`Serial communication established with ${deviceName}`);
    },

    /**
     * Sets the program into a disconnected state
     */
    setDisconnected() {
      this.deviceName = '';
      this.isConnected = false;
      this.showSnackbar('Serial communication disconnected');
    },

    /**
     * Processes serial input
     */
    processData(data) {
      // Keep a track of the old data
      const oldMode = this.currentMode;

      if (data[0] === '$') {
        // An LCD display
        this.bus.$emit('lcd-text', data);
        return;
      }

      // Command is the first character
      const command = data[0];
      switch (command) {
        case 'r':
          // Resistance mode
          this.currentMode = 'resistance';
          break;

        case 'v':
          // Voltage mode
          this.currentMode = 'voltage';
          break;

        case 'c':
          // Current mode
          this.currentMode = 'current';
          break;

        case '!':
          // Mode not set yet
          break;

        case 'a':
          // Update ac mode
          const ac = data.split(' ')[1];

          if (ac === '1') {
            this.bus.$emit('update-ac-mode', true);
          } else {
            this.bus.$emit('update-ac-mode', false);
          }

          break;

        case '$':
          // LCD Text mode
          this.bus.$emit('lcd-text', data);
          break;

        case '#':
          // Data entry mode
          const value = parseFloat(data.split(' ')[1]);
          // const sampleNum = parseFloat(data.split(' ')[2]);
          const date = moment();

          if (!isNaN(value)) {
            this.addEntry(date, value);
          }
          break;

        default:
          break;
      }

      if (oldMode !== this.currentMode) {
        this.bus.$emit('set-range', this.currentMode);
      }
    },

    /**
     * Deletes the data stored in the application
     */
    deleteData() {
      this.data = [];
    },

    /**
     * Toggles the pause mode
     */
    togglePause() {
      if (this.isPaused) {
        this.isPaused = false;
        this.bus.$emit('pause-off');
      } else {
        this.isPaused = true;
        this.bus.$emit('pause-on');
      }
    },
  },

  computed: {

  },

  created() {
    this.bus = new Vue();

    this.date = moment();
  },

  mounted() {
    this.socket = io({ path: `${window.location.pathname}socket.io` });

    const scope = this;
    this.socket.on('update', (data) => {
      scope.queues = data;
    });

    this.socket.on('portslist', (ports) => {
      this.bus.$emit('portslist', ports);
    });

    /**
     * Device status events
     */
    this.socket.on('deviceconnected', this.setConnected);
    this.socket.on('devicedisconnected', this.setDisconnected);
    this.socket.on('devicedata', this.processData);

    /**
     * UI events
     */
    this.bus.$on('toggle-pause', this.togglePause);
    this.bus.$on('delete-data', this.deleteData);
    this.bus.$on('show-snackbar', (message) => { this.showSnackbar(message); });
    this.bus.$on('set-data', this.setData);

    this.bus.$on('set-brightness', (data) => {
      this.socket.emit('set-brightness', data);
    });

    /**
     * Logging modes
     */
    this.bus.$on('start-log', (data) => {
      this.socket.emit('start-log', data);
    });

    this.socket.on('logging-finished', () => {
      this.bus.$emit('logging-finished');
    });

    /**
     * AC Modes
     */
    this.bus.$on('set-ac-mode', (data) => {
      this.socket.emit('set-ac-mode', data);
    });

    this.socket.on('update-ac-mode', (data) => {
      this.bus.$emit('update-ac-mode', data);
    });

    /**
     * Set range request
     */
    this.bus.$on('set-range', (newRange) => {
      this.currentMode = newRange;
      this.socket.emit('set-range', newRange);
    });

    /**
     * Handle randome data
     */
    this.bus.$on('start-random-data', this.startRandomData);
    this.bus.$on('stop-random-data', this.stopRandomData);

    /**
     * Set port request
     */
    this.bus.$on('setport', (port) => {
      this.socket.emit('setport', port);
    });

    /**
     * Device disconnect event
     */
    this.bus.$on('device-disconnect', () => {
      this.socket.emit('device-disconnect');
    });

    /**
     * Sampling rate request
     */
    this.bus.$on('sampling-rate', (sampleRate) => {
      this.socket.emit('sampling-rate', sampleRate);
    });

    /**
     * Get ports request
     */
    this.bus.$on('get-ports', () => {
      this.socket.emit('getports');
    });
  },
});
