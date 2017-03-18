/* global d3, moment, io, window, Vue */

Vue.component('chart', {
  template: `
    <div class="chart">
      <svg ref="chart" id="vis" class="d3-chart"></svg>
    </div> 
  `,

  props: ['data', 'bus'],

  data() {
    return {
      chart: null,
      g: null,
      height: 0,
      width: 0,
      margin: { top: 20, right: 20, bottom: 30, left: 50 },
      // The time scale on the x axis
      xscale: null,
      yscale: null,

      // Masks
      numOfMaskPoints: 10,
      topMasks: [],
      bottomMasks: [],

      // Configuration
      dataLimit: 10,

      yLabelObj: null,
    };
  },

  methods: {
    drawMasks() {
      const topMask = d3.line()
        .x((d) => { return d.x; })
        .y((d) => { return d.y; })
        .curve(d3.curveMonotoneX);

      const bottomMask = d3.line()
        .x((d) => { return d.x; })
        .y((d) => { return d.y; })
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
        .attr('cy', (d) => { return d.y; })
        .style('stroke', 'blue')
        .call(d3.drag().on('drag', this.moveTopMaskCircle));

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
        .attr('cy', (d) => { return d.y; })
        .style('stroke', 'red')
        .call(d3.drag().on('drag', this.moveBottomMaskCircle));
    },

    moveTopMaskCircle(d) {
      const m = d3.mouse(this.g.node());
      const item = this.topMasks[this.topMasks.indexOf(d)];
      item.y = m[1];
      item.x = m[0];

      this.drawMasks();
    },

    moveBottomMaskCircle(d) {
      const m = d3.mouse(this.g.node());
      const item = this.bottomMasks[this.bottomMasks.indexOf(d)];
      item.y = m[1];
      item.x = m[0];

      this.drawMasks();
    },

    /**
     * Processes and prepares data from plotting
     * Includes limiting the amount of data to plot
     */
    getData() {

      // Return at most 'dataLimit' elements from the end of data
      let limitedData = this.data.slice(Math.max(this.data.length - this.dataLimit, 0));

      const translated = limitedData.map((item) => {
        const translatedItem = {
          y: this.yscale(item.value),
          x: this.xscale(item.date),
        };

        return translatedItem;
      });

      const data = {
        topMask: this.topMasks,
        bottomMask: this.bottomMasks,
        translatedData: translated,
        rawData: limitedData,
      };

      return data;
    },

    drawChart() {
      // Delete any artifacts
      this.chart.selectAll('*').remove();

      this.g = this.chart.append('g').attr('transform', `translate(${this.margin.left}, ${this.margin.top})`);

      const x = d3.scaleTime()
        .rangeRound([0, this.width]);

      // Set the xscale
      this.xscale = x;

      const y = d3.scaleLinear()
        .rangeRound([this.height, 0]);

      // Set the yscale
      this.yscale = y;

      const data = this.getData().rawData;

      // The line for the data
      const line = d3.line()
        .x((d) => { return x(d.date); })
        .y((d) => { return y(d.value); });

      x.domain(d3.extent(data, (d) => { return d.date; }));
      // y.domain(d3.extent(this.data, (d) => { return d.value; }));

      // TODO: This is gonna cause issues with the masks
      // TUTOR QUESTION
      // y.domain([0, 15]);
      y.domain([d3.min(data, (d) => { return d.value; }) - 4, d3.max(data, (d) => { return d.value }) + 4]);

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
        .attr('y', 6);

      this.yLabelObj = this.g.append('text')
        .attr('transform', 'rotate(-90)')
        .attr('y', 0 - this.margin.left)
        .attr('x', 0 - (this.height / 2))
        .attr('dy', '1em')
        .style('text-anchor', 'middle')
        .text('Data');

      // Data
      this.g.append('path')
        .datum(data)
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
      // TODO: Refactor to arrow notation, but allow 'this' keyword
      // Should this be a vue method?
      this.chart.on('mousemove', function() {
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

    /**
     * Sets up the masks
     */
    setupMask() {
      this.topMasks = [
        { x: 0, y: 0 },
      ];

      this.bottomMasks = [
        { x: 0, y: this.height },
      ];

      for (let i = 0; i < this.numOfMaskPoints; i += 1) {
        this.topMasks.push({
          x: ((i / this.numOfMaskPoints) * this.width) + 1,
          y: 10,
        });

        this.bottomMasks.push({
          x: ((i / this.numOfMaskPoints) * this.width) + 1,
          y: this.height - 10,
        });
      }

      this.topMasks.push({ x: this.width, y: 0 });
      this.bottomMasks.push({ x: this.width, y: this.height });
    },

    /**
     * Sets up the chart for display
     */
    setupChart() {
      this.chart = d3.select(this.$refs.chart);

      this.height = this.$refs.chart.getBoundingClientRect().height
        - this.margin.top - this.margin.bottom;

      this.width = this.$refs.chart.getBoundingClientRect().width
        - this.margin.right - this.margin.left;

      // TODO: Refactor this, we don't need it to work like this
      // this.setupMask();

      this.drawChart();
    },

    setMasks(masks) {
      // Clear the masks
      this.topMasks = [];
      this.bottomMasks = [];

      masks.forEach((mask) => {
        console.log(this.topMasks);
        if (mask.mask === 'high') {
          this.topMasks.push(mask);
        } else if (mask.mask === 'low') {
          this.bottomMasks.push(mask);
        } else {
          console.log(`Tried to parse mask '${mask.mask}'`);
        }
      });

      this.bus.$emit('show-snackbar', 'Masks loaded from file');
    },

    setYLabel(text) {
      this.yLabelObj.text(text);
    },

  },

  watch: {
    topMasks: {
      handler: function() {
        const test = this.getData();
        collision.checkCollision(test.translatedData, test.topMask, test.bottomMask);
      },
      deep: true,
    },

    bottomMasks: {
      handler: function() {
        const test = this.getData();
        collision.checkCollision(test.translatedData, test.topMask, test.bottomMask);
      },
      deep: true,
    },

    data() {
      this.drawChart();

      // Testing
      const test = this.getData();
      collision.checkCollision(test.translatedData, test.topMask, test.bottomMask);
    },
  },

  mounted() {
    this.setupChart();

    const scope = this;
    window.addEventListener('resize', () => {
      scope.setupChart();
    });

    this.bus.$on('set-masks', (masks) => { this.setMasks(masks); });
  },
});
