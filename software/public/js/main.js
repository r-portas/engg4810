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

      socket: null,

      intervalRef: null,

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
      const isoString = date.toISOString();
      const parseTime = d3.isoParse;
      this.data.push({ date: parseTime(isoString), value });
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
      this.data = data;
    },

    startRandomData() {
      const date = moment();

      this.intervalRef = setInterval(() => {
        const rand = Math.floor((Math.random() * 100) + 1);
        this.addEntry(date.add(1, 'hours'), rand);
      }, 1000);
    },

    stopRandomData() {
      clearInterval(this.intervalRef);
    },

  },

  computed: {

  },

  created() {
    this.bus = new Vue();
  },

  mounted() {
    this.socket = io({ path: `${window.location.pathname}socket.io` });

    const scope = this;
    this.socket.on('update', (data) => {
      scope.queues = data;
    });

    // TESTING CODE
    // const date = moment();

    // setInterval(() => {
    //   const rand = Math.floor((Math.random() * 100) + 1);
    //   scope.addEntry(date.add(1, 'hours'), rand);
    // }, 1000);

    // END TESTING
    

    this.bus.$on('show-snackbar', (message) => { this.showSnackbar(message); });
    this.bus.$on('set-data', this.setData);

    this.bus.$on('start-random-data', this.startRandomData);
    this.bus.$on('stop-random-data', this.stopRandomData);
  },
});
