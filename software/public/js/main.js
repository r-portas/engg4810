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
      socket: null,

      data: [],
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

  },

  computed: {

  },

  mounted() {
    this.socket = io({ path: `${window.location.pathname}socket.io` });

    const scope = this;
    this.socket.on('update', (data) => {
      scope.queues = data;
    });

    // TESTING CODE
    const date = moment();

    this.addEntry(date.add(1, 'hours'), 5);
    this.addEntry(date.add(1, 'hours'), 4);
    this.addEntry(date.add(1, 'hours'), 6);

    /*
    setInterval(() => {
      const rand = Math.floor((Math.random() * 100) + 1);
      scope.addEntry(date.add(1, 'hours'), rand);
    }, 1000);
    */

    // END TESTING
  },
});
