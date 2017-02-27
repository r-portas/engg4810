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
      var parseTime = d3.timeParse('%d-%b-%y');
      this.data.push({date: parseTime(date), value: value});

      this.makeChart();
    },

    makeChart() {

      var chart = d3.select(this.$refs.chart);

      // Delete any artifacts
      chart.selectAll('*').remove();

      var margin = {top: 20, right: 20, bottom: 30, left: 50};
      var height = this.$refs.chart.getBoundingClientRect().height - margin.top - margin.bottom;
      var width = this.$refs.chart.getBoundingClientRect().width - margin.right - margin.left;
      var g = chart.append('g').attr('transform', 'translate(' + margin.left + ', ' + margin.top + ')');

      var x = d3.scaleTime()
        .rangeRound([0, width]);

      var y = d3.scaleLinear()
        .rangeRound([height, 0]);

      var line = d3.line()
        .x(function(d) { return x(d.date) })
        .y(function(d) { return y(d.value) });

      x.domain(d3.extent(this.data, function(d) { return d.date }));
      y.domain(d3.extent(this.data, function(d) { return d.value }));

      // X axis
      g.append('g')
        .attr('transform', 'translate(0, ' + height + ')')
        .call(d3.axisBottom(x))
        .select(".domain")
        .remove();

      // Y axis
      g.append('g')
        .call(d3.axisLeft(y))
        .append("text")
        .attr("fill", "#000")
        .attr("transform", "rotate(-90)")
        .attr("y", 6)
        .attr("dy", "0.71em")
        .attr("text-anchor", "end")
        .text("Reading");

      // Data
      g.append('path')
        .datum(this.data)
        .attr('fill', 'none')
        .attr('stroke', 'steelblue')
        .attr('stroke-width', 1.5)
        .attr('d', line);
    }
  },

  computed: {

  },

  mounted: function() {
    this.socket = io({path: window.location.pathname + 'socket.io'});

    var scope = this;
    this.socket.on('update', function(data) {
      scope.queues = data;
    });

    this.makeChart();

    this.addEntry('20-Apr-07', 5);
    this.addEntry('21-Apr-07', 4);
    this.addEntry('22-Apr-07', 6);

    var scope = this;
    setTimeout(function() {
      scope.addEntry('23-Apr-07', 5);
    }, 3000);

    setTimeout(function() {
      scope.addEntry('24-Apr-07', 6);
    }, 6000);

    var scope = this;
    window.onresize = function(e) {
      scope.makeChart();
    }
  }
});
