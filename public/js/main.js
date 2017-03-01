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

      // The line for the data
      var line = d3.line()
        .x(function(d) { return x(d.date) })
        .y(function(d) { return y(d.value) });

      var numOfMaskPoints = 10;

      var mask = [
        {x: 0, top: 0, bottom: height}
      ];

      for (var i = 0; i < numOfMaskPoints; i++) {
        mask.push({
          x: (i/numOfMaskPoints) * width + 1,
          top: 10,
          bottom: height - 10
        });
      }

      mask.push({x: width, top: 0, bottom: height});

      var topMask = d3.line()
        .x(function(d) { return d.x })
        .y(function(d) { return d.top })
        .curve(d3.curveCatmullRom.alpha(0.5));

      var bottomMask = d3.line()
        .x(function(d) { return d.x })
        .y(function(d) { return d.bottom });

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

      // Top mask
      g.append('path')
        .classed('topmask', true)
        .datum(mask)
        .attr('fill', 'lightblue')
        .attr('stroke', 'lightblue')
        .attr('stroke-width', 1.5)
        .attr('d', topMask)
        .enter().append('circle')
          .attr('class', 'line');

      // Draw the crosshair
      g.append('line')
        .classed('x', true)
        .style('stroke', 'black')
        .style('stroke-width', '1px')
        .style('stroke-dasharray', '3 3');

      g.append('line')
        .classed('y', true)
        .style('stroke', 'black')
        .style('stroke-width', '1px')
        .style('stroke-dasharray', '3 3');

      // The text value
      g.append('text')
        .classed('description', true);

      g.append('circle')
        .attr('class', 'y')
        .style('fill', 'none')
        .style('stroke', 'blue')
        .attr('r', 4);

      var scope = this;
      chart.on('mousemove', function() {
        var bisectDate = d3.bisector(function(d) {
          return d.date;
        }).left;

        var mouse = d3.mouse(this);
        var x0 = x.invert(mouse[0]);

        var i = bisectDate(scope.data, x0, 1);
        if (i < scope.data.length) {
          var d0 = scope.data[i - 1];
          var d1 = scope.data[i];
          // TODO: Refactor
          var d = x0 - d0.date > d1.date - x0 ? d1 : d0;

          g.select('circle.y')
            .attr('transform', 'translate(' + x(d.date) + ', ' + y(d.value) + ')');
          
          g.select('line.x')
            .attr('x1', 0)
            .attr('x2', x(d.date))
            .attr('y1', y(d.value))
            .attr('y2', y(d.value));

          g.select('line.y')
            .attr('x1', x(d.date))
            .attr('x2', x(d.date))
            .attr('y1', height)
            .attr('y2', y(d.value));

          g.select('text.description')
            .attr('x', x(d.date) + 10)
            .attr('y', y(d.value) - 10)
            .text(d.value);
        }

      });
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
      scope.makeChart();
    }
  }
});
