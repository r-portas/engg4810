/**
 * The main vue instance
 *
 * @author Roy Portas <royportas@gmail.com>
 */

var app = new Vue({
  el: '#app',

  data: function() {
    return {
      socket: null,

      data: []
    };
  },

  methods: {
    /**
     * Shows a notification through the snackbar
     */
    showSnackbar(message) {
      var sb = this.$refs.snackbar;
      var data = {
        message: message,
      };
      sb.MaterialSnackbar.showSnackbar(data);
    },

    addEntry(date, value) {
      var isoString = date.toISOString();
      var parseTime = d3.isoParse;
      this.data.push({date: parseTime(isoString), value: value});
    },

  },

  computed: {

  },

  mounted: function() {
    this.socket = io({path: window.location.pathname + 'socket.io'});

    var scope = this;
    this.socket.on('update', function(data) {
      scope.queues = data;
    });

    // TESTING CODE
    var date = moment();

    this.addEntry(date.add(1, 'hours'), 5);
    this.addEntry(date.add(1, 'hours'), 4);
    this.addEntry(date.add(1, 'hours'), 6);

    var scope = this;
    setInterval(function() {
      var rand = Math.floor((Math.random() * 100) + 1);
      scope.addEntry(date.add(1, 'hours'), rand);
    }, 1000);

    // END TESTING

    var scope = this;
    window.onresize = function(e) {
      // scope.makeChart();
    }
  }
});
