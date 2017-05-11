/* global d3, moment, io, window, Vue, collision */

Vue.component('chart', {
  template: `
    <div class="mdl-card mdl-shadow--4dp chart mm-card">
      <svg ref="chart" id="vis" class="d3-chart"></svg>

      <input class="mdl-slider mdl-js-slider" type="range"
        min="0" :max="dataItems" v-model="scrollIndex" tabindex="0">
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

      // The index to scroll to
      scrollIndex: 0,

      // Configuration
      dataLimit: 10,
      voltageRange: [-12, 12],
      currentRange: [-200, 200],
      //resistanceRange: [0, 1000000],
      resistanceRange: [0, 100000],
      logicRange: [-0.5, 1.5],

      selectedRange: 'voltage',

      rangeLabel: {
        voltage: 'Voltage (V)',
        current: 'Current (mA)',
        resistance: 'Resistance (ohms)',
        logic: 'Logic Level (digital)',
      },

      yLabelObj: null,
    };
  },

  computed: {
    dataItems() {
      return Math.max(this.data.length, 1);
    },
  },

  methods: {
    drawMasks() {
      this.drawSingleMask(this.topMasks, 'topmask', 'blue', 'rgba(1, 0, 255, 0.3)', this.moveTopMaskCircle);
      this.drawSingleMask(this.bottomMasks, 'bottommask', 'red', 'rgba(255, 0, 1, 0.3)', this.moveBottomMaskCircle);
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

      const beginIndex = Math.max(this.scrollIndex - this.dataLimit, 0);
      const endIndex = Math.min(this.dataLimit, this.data.length) + beginIndex;

      const limitedData = this.data.slice(beginIndex, endIndex);

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

    checkProximity(x, y, point) {
      const p = 20;

      if ((x - p) < point.x && point.x < (x + p)) {
        if ((y - p) < point.y && point.y < (y + p)) {
          return true;
        }
      }

      return false;
    },

    /**
     * Deletes the masks at the specified x, y point
     */
    deleteMask(x, y) {
      for (let i = 0; i < this.topMasks.length; i += 1) {
        const point = this.topMasks[i];
        if (this.checkProximity(x, y, point)) {
          this.topMasks.splice(i, 1);
          this.bus.$emit('show-snackbar', 'Mask point deleted');
        }
      }

      for (let i = 0; i < this.bottomMasks.length; i += 1) {
        const point = this.bottomMasks[i];
        if (this.checkProximity(x, y, point)) {
          this.bottomMasks.splice(i, 1);
          this.bus.$emit('show-snackbar', 'Mask point deleted');
        }
      }
    },

    drawChart() {
      // Delete any artifacts
      this.chart.selectAll('*').remove();
      this.chart.selectAll('.line').remove();

      this.g = this.chart.append('g').attr('transform', `translate(${this.margin.left}, ${this.margin.top})`);

      const x = d3.scaleTime()
        .rangeRound([0, this.width]);

      // Set the xscale
      this.xscale = x;

      const y = d3.scaleLinear()
        .rangeRound([this.height, 0]);

      // Set the yscale
      this.yscale = y;

      // Apply the scrollIndex
      const data = this.getData().rawData;

      // The line for the data
      const line = d3.line()
        .x((d) => { return x(d.date); })
        .y((d) => { return y(d.value); });

      x.domain(d3.extent(data, (d) => { return d.date; }));
      // y.domain(d3.extent(this.data, (d) => { return d.value; }));

      // y.domain([0, 15]);

      switch (this.selectedRange) {
        case 'voltage':
          y.domain(this.voltageRange);
          break;

        case 'current':
          y.domain(this.currentRange);
          break;

        case 'resistance':
          y.domain(this.resistanceRange);
          break;

        case 'logic':
          y.domain(this.logicRange);
          break;

        case 'auto':
          console.log('Auto range not implemented yet');
          break;

        default:
          console.log(`Range '${this.selectedRange}' is not supported`);
          break;
      }

      const xAxis = d3.axisBottom(x).tickFormat(d3.timeFormat('%H'));

      // X axis
      this.g.append('g')
        .attr('transform', `translate(0, ${this.height})`)
        .call(xAxis)
        .select('.domain')
        .remove();

      // Y axis
      this.g.append('g')
        .call(d3.axisLeft(y))
        .append('text')
        .attr('fill', '#000')
        .attr('transform', 'rotate(-90)')
        .attr('y', 6);

      // Y axis label
      this.yLabelObj = this.g.append('text')
        .attr('transform', 'rotate(-90)')
        .attr('y', 0 - this.margin.left)
        .attr('x', 0 - (this.height / 2))
        .attr('dy', '1em')
        .style('text-anchor', 'middle')
        .text(this.rangeLabel[this.selectedRange]);

      // Data
      this.g.append('path')
        .datum(data)
        .classed('line', true)
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

      this.chart.on('click', function() {
        const mouse = d3.mouse(this);

        // Fix the margin offsets
        mouse[0] -= scope.margin.left;
        mouse[1] -= scope.margin.top;

        // If the shift key is pressed, add to the bottom mask
        if (d3.event.ctrlKey || d3.event.shiftKey) {
          scope.addMask(mouse[0], mouse[1], 'low');
        } else if (d3.event.altKey) {
          scope.deleteMask(mouse[0], mouse[1]);
        } else {
          scope.addMask(mouse[0], mouse[1], 'high');
        }

        scope.drawMasks();
      });

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

          const sampleNum = scope.data.indexOf(d) + 1;
          const date = moment(d.date).format('LT');
          const tooltipText = `${d.value}, ${sampleNum}, ${date}`;

          scope.g.select('text.description')
            .attr('x', x(d.date) - 50)
            .attr('y', y(d.value) - 10)
            .html(tooltipText);
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

    /**
     * Updates the range of the chart and redraws it
     */
    setRange(range) {
      this.selectedRange = range;

      this.drawChart();
    },

    /**
     * Adds a single mask
     */
    addMask(x, y, mask) {
      const m = {
        x,
        y,
        mask,
      };

      if (mask === 'high') {
        this.topMasks.push(m);
      } else if (mask === 'low') {
        this.bottomMasks.push(m);
      } else {
        console.log(`Tried to parse mask '${mask}'`);
      }
    },

    /**
     * Draws a single mask
     */
    drawSingleMask(maskData, cssSelector, lineColor, fillColor, moveMaskFunction) {
      const mask = d3.line()
        .x((d) => { return d.x; })
        .y((d) => { return d.y; })
        .curve(d3.curveMonotoneX);

      this.g.selectAll(`.${cssSelector}`).remove();

      // Draw the dashed lines
      const minMaskPoint = this.getMinimumMaskValue(maskData);
      if (minMaskPoint !== null) {
        this.g.append('line')
          .classed(cssSelector, true)
          .attr('x1', 0)
          .attr('y1', minMaskPoint.y)
          .attr('x2', minMaskPoint.x)
          .attr('y2', minMaskPoint.y)
          .attr('stroke', lineColor)
          .attr('stroke-width', 1.5)
          .style('stroke-dasharray', ('3, 3'));
      }

      const maxMaskPoint = this.getMaximumMaskValue(maskData);
      if (maxMaskPoint !== null) {
        this.g.append('line')
          .classed(cssSelector, true)
          .attr('x1', this.width)
          .attr('y1', maxMaskPoint.y)
          .attr('x2', maxMaskPoint.x)
          .attr('y2', maxMaskPoint.y)
          .attr('stroke', lineColor)
          .attr('stroke-width', 1.5)
          .style('stroke-dasharray', ('3, 3'));
      }


      // If we have valid points, draw the area
      if (maxMaskPoint !== null && minMaskPoint !== null) {
        const minPoint = {
          x: 0,
          y: minMaskPoint.y,
        };

        const maxPoint = {
          x: this.width,
          y: maxMaskPoint.y,
        };

        let allPoints = [];
        if (cssSelector === 'bottommask') {
          allPoints = [
            { x: 0, y: this.height },
            minPoint,
          ];

          allPoints = allPoints.concat(maskData);
          allPoints = allPoints.concat([
            maxPoint,
            { x: this.width, y: this.height },
          ]);
        } else if (cssSelector === 'topmask') {
          allPoints = [
            { x: 0, y: 0 },
            minPoint,
          ];

          allPoints = allPoints.concat(maskData);
          allPoints = allPoints.concat([
            maxPoint,
            { x: this.width, y: 0 },
          ]);
        }

        // Draw the fill area
        this.g.append('path')
          .classed(cssSelector, true)
          .datum(allPoints)
          .attr('fill', fillColor)
          .attr('stroke-width', 0)
          .attr('d', mask);
      }

      // Top mask circles
      this.g.selectAll(`circle${cssSelector}`)
        .data(maskData)
        .enter()
        .append('circle')
        .classed(cssSelector, true)
        .attr('r', 6.5)
        .attr('cx', (d) => { return d.x; })
        .attr('cy', (d) => { return d.y; })
        .style('stroke', lineColor)
        .call(d3.drag().on('drag', moveMaskFunction));
    },


    /**
     * Returns the minimum value for the mask
     */
    getMinimumMaskValue(masks) {
      if (masks.length === 0) {
        return null;
      }

      return masks.reduce((p, v) => {
        if (p.x < v.x) {
          return p;
        }

        return v;
      });
    },

    /**
     * Returns the maximum value for the mask
     */
    getMaximumMaskValue(masks) {
      if (masks.length === 0) {
        return null;
      }

      return masks.reduce((p, v) => {
        if (p.x > v.x) {
          return p;
        }

        return v;
      });
    },

    setMasks(masks) {
      // Clear the masks
      this.topMasks = [];
      this.bottomMasks = [];

      masks.forEach((mask) => {
        if (mask.mask === 'high') {
          this.topMasks.push(mask);
        } else if (mask.mask === 'low') {
          this.bottomMasks.push(mask);
        } else {
          console.log(`Tried to parse mask '${mask.mask}'`);
        }
      });

      if (masks.length === 0) {
        this.bus.$emit('show-snackbar', 'Masks cleared');
      } else {
        this.bus.$emit('show-snackbar', 'Masks loaded from file');
      }

      this.drawMasks();
    },

    translatedCollisionScale(collisions) {
      collisions.map((c) => {
        const temp = c;
        temp.startCollision = this.xscale.invert(c.dataItem.point1.x);
        temp.endCollision = this.xscale.invert(c.dataItem.point2.x);
        return temp;
      });

      return collisions;
    },

    /**
     * Processes the collisions
     *
     * Checks for collisions and emits an 'collisions' event on
     * the main bus along with a array of collision data
     */
    processCollisions() {
      const data = this.getData();
      let collisions = collision.checkCollision(data.translatedData, data.topMask, data.bottomMask);
      collisions = this.translatedCollisionScale(collisions);
      this.bus.$emit('collisions', collisions);
    },

    /**
     * Translates a set of masks to the required format
     */
    translateMasks(masks) {
      return masks.map((m) => {
        const temp = {};
        temp.mask = m.mask;
        temp.x = this.xscale.invert(m.x);
        temp.y = m.y;

        return temp;
      });
    },

    getMasks() {
      this.bus.$emit('masks', {
        topMasks: this.topMasks,
        bottomMasks: this.bottomMasks,
        measurement: this.selectedRange,
      });
    },

  },

  watch: {
    topMasks: {
      handler() {
        this.processCollisions();
      },
      deep: true,
    },

    bottomMasks: {
      handler() {
        this.processCollisions();
      },
      deep: true,
    },

    scrollIndex() {
      this.drawChart();
    },

    data() {
      // Set the scroll index to the end
      this.scrollIndex = this.data.length;

      this.drawChart();
      this.processCollisions();
    },
  },

  mounted() {
    this.setupChart();

    const scope = this;
    window.addEventListener('resize', () => {
      scope.setupChart();
    });

    this.bus.$on('set-masks', (masks) => { this.setMasks(masks); });
    this.bus.$on('set-range', (range) => { this.setRange(range); });
    this.bus.$on('get-masks', () => this.getMasks());
  },
});
