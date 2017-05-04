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

    addEntry(date, value) {
      // Only add data if we aren't paused
      if (this.isPaused === false) {
        const isoString = date.toISOString();
        const parseTime = d3.isoParse;
        this.data.push({ date: parseTime(isoString), value });
      }
    },
/**
     * Checks if the current tab is active
     */
    checkTabActive(tabName) {
      return tabName === this.currentTab;
    },

    setData(data) {
      for (let item of data) {
        const date = moment(item.date);
        this.addEntry(date, item.value);
      }
    },

    startRandomData() {
      this.intervalRef = setInterval(() => {
        this.date.add(1, 'hours');
        const rand = Math.floor((Math.random() * 20) + 1) - 10;
        this.addEntry(this.date, rand);
      }, 1000);
    },

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
      console.log(data);
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
      } else {
        this.isPaused = true;
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

    this.socket.on('deviceconnected', this.setConnected);
    this.socket.on('devicedisconnected', this.setDisconnected);
    this.socket.on('devicedata', this.processData);

    this.bus.$on('toggle-pause', this.togglePause);
    this.bus.$on('delete-data', this.deleteData);
    this.bus.$on('show-snackbar', (message) => { this.showSnackbar(message); });
    this.bus.$on('set-data', this.setData);

    this.bus.$on('set-range', (newRange) => {
      this.socket.emit('set-range', newRange);
    });

    this.bus.$on('start-random-data', this.startRandomData);
    this.bus.$on('stop-random-data', this.stopRandomData);

    this.bus.$on('setport', (port) => {
      this.socket.emit('setport', port);
    });

    this.bus.$on('device-disconnect', () => {
      this.socket.emit('device-disconnect');
    });

    this.bus.$on('get-ports', () => {
      this.socket.emit('getports');
    });

  },
});
