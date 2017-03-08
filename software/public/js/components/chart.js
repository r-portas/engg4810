/* global d3, moment, io, window, Vue */

Vue.component('chart', {
  template: `
    <div class="chart">
      <svg ref="chart" id="vis" class="d3-chart"></svg>
    </div> 
  `,

  props: ['data'],

  data() {
    return {
      bus: null,
      chart: null,
      g: null,
      height: 0,
      width: 0,
      margin: { top: 20, right: 20, bottom: 30, left: 50 },

      // Masks
      numOfMaskPoints: 10,
      topMasks: [],
      bottomMasks: [],
    };
  },

  methods: {
    drawMasks() {
      const topMask = d3.line()
        .x((d) => { return d.x; })
        .y((d) => { return d.top; })
        .curve(d3.curveCardinal);

      const bottomMask = d3.line()
        .x((d) => { return d.x; })
        .y((d) => { return d.bottom; })
        .curve(d3.curveCatmullRom.alpha(0.5));

      // Remove the old masks
      this.g.selectAll('.topmask').remove();
      this.g.selectAll('.bottommask').remove();

      // Top mask
      this.g.append('path')
        .classed('topmask', true)
        .datum(this.topMasks)
        .attr('fill', 'rgba(0, 0, 255, 0.3)')
        .attr('stroke', 'blue')
        .attr('stroke-width', 1.5)
        .attr('d', topMask)
        .enter()
        .append('circle')
          .attr('class', 'line');

      // Top mask circles
      this.g.selectAll('circle.topmask')
        .data(this.topMasks.slice(1, this.topMasks.length - 1))
        .enter()
        .append('circle')
        .classed('topmask', true)
        .attr('r', 6.5)
        .attr('cx', (d) => { return d.x; })
        .attr('cy', (d) => { return d.top; })
        .style('stroke', 'blue')
        .call(d3.drag()
          .on('drag', this.moveTopMaskCircle),
        );

      // Bottom mask
      this.g.append('path')
        .classed('bottommask', true)
        .datum(this.bottomMasks)
        .attr('fill', 'rgba(255, 0, 0, 0.3)')
        .attr('stroke', 'red')
        .attr('stroke-width', 1.5)
        .attr('d', bottomMask)
        .enter()
        .append('circle')
          .attr('class', 'line');

      // Bottom mask circles
      this.g.selectAll('circle.bottommask')
        .data(this.bottomMasks.slice(1, this.bottomMasks.length - 1))
        .enter()
        .append('circle')
        .classed('bottommask', true)
        .attr('r', 6.5)
        .attr('cx', (d) => { return d.x; })
        .attr('cy', (d) => { return d.bottom; })
        .style('stroke', 'red')
        .call(d3.drag()
          .on('drag', this.moveBottomMaskCircle),
        );
    },

    moveTopMaskCircle(d) {
      const m = d3.mouse(this.g.node());
      d.top = m[1];
      d.x = m[0];

      this.drawMasks();
    },

    moveBottomMaskCircle(d) {
      const m = d3.mouse(this.g.node());
      d.bottom = m[1];
      d.x = m[0];

      this.drawMasks();
    },

    drawChart() {
      // Delete any artifacts
      this.chart.selectAll('*').remove();

      this.g = this.chart.append('g').attr('transform', `translate(${this.margin.left}, ${this.margin.top})`);

      const x = d3.scaleTime()
        .rangeRound([0, this.width]);

      const y = d3.scaleLinear()
        .rangeRound([this.height, 0]);

      // The line for the data
      const line = d3.line()
        .x((d) => { return x(d.date); })
        .y((d) => { return y(d.value); });

      x.domain(d3.extent(this.data, (d) => { return d.date; }));
      y.domain(d3.extent(this.data, (d) => { return d.value; }));

      // X axis
      this.g.append('g')
        .attr('transform', `translate(0, ${this.height})`)
        .call(d3.axisBottom(x))
        .select('.domain')
        .remove();

      // Y axis
      this.g.append('g')
        .call(d3.axisLeft(y))
        .append('text')
        .attr('fill', '#000')
        .attr('transform', 'rotate(-90)')
        .attr('y', 6)
        .attr('dy', '0.71em')
        .attr('text-anchor', 'end')
        .text('Reading');

      // Data
      this.g.append('path')
        .datum(this.data)
        .attr('fill', 'none')
        .attr('stroke', 'steelblue')
        .attr('stroke-width', 1.5)
        .attr('d', line);

      // Draw the crosshair
      this.g.append('line')
        .classed('x', true)
        .style('stroke', 'black')
        .style('stroke-width', '1px')
        .style('stroke-dasharray', '3 3');

      this.g.append('line')
        .classed('y', true)
        .style('stroke', 'black')
        .style('stroke-width', '1px')
        .style('stroke-dasharray', '3 3');

      // The text value
      this.g.append('text')
        .classed('description', true);

      this.g.append('circle')
        .attr('class', 'y')
        .style('fill', 'none')
        .style('stroke', 'blue')
        .attr('r', 4);

      const scope = this;
      this.chart.on('mousemove', () => {
        const bisectDate = d3.bisector((d) => {
          return d.date;
        }).left;

        const mouse = d3.mouse(this);
        const x0 = x.invert(mouse[0]);

        const i = bisectDate(scope.data, x0, 1);
        if (i < scope.data.length) {
          const d0 = scope.data[i - 1];
          const d1 = scope.data[i];
          // TODO: Refactor
          const d = x0 - d0.date > d1.date - x0 ? d1 : d0;

          scope.g.select('circle.y')
            .attr('transform', `translate(${x(d.date)}, ${y(d.value)})`);

          scope.g.select('line.x')
            .attr('x1', 0)
            .attr('x2', x(d.date))
            .attr('y1', y(d.value))
            .attr('y2', y(d.value));

          scope.g.select('line.y')
            .attr('x1', x(d.date))
            .attr('x2', x(d.date))
            .attr('y1', scope.height)
            .attr('y2', y(d.value));

          scope.g.select('text.description')
            .attr('x', x(d.date) + 10)
            .attr('y', y(d.value) - 10)
            .text(d.value);
        }
      });

      this.drawMasks();
    },

    setupChart() {
      this.chart = d3.select(this.$refs.chart);

      this.height = this.$refs.chart.getBoundingClientRect().height
        - this.margin.top - this.margin.bottom;

      this.width = this.$refs.chart.getBoundingClientRect().width
        - this.margin.right - this.margin.left;

      this.topMasks = [
        { x: 0, top: 0 },
      ];

      this.bottomMasks = [
        { x: 0, bottom: this.height },
      ];

      for (var i = 0; i < this.numOfMaskPoints; i++) {
        this.topMasks.push({
          x: (i / this.numOfMaskPoints) * this.width + 1,
          top: 10
        });

        this.bottomMasks.push({
          x: (i / this.numOfMaskPoints) * this.width + 1,
          bottom: this.height - 10
        });
      }

      this.topMasks.push({x: this.width, top: 0});
      this.bottomMasks.push({x: this.width, bottom: this.height});

      this.drawChart();
    }

  },

  watch: {
    data: function() {
      this.drawChart();
    }
  },

  mounted: function() {
    this.setupChart(); 

    var scope = this;
    window.addEventListener('resize', function(e) {
      console.log('Redrawing');
      scope.setupChart(); 
    });
  }
});
