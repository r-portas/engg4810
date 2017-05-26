/* global Vue, Papa, moment, Blob, saveAs, collision */

Vue.component('settings', {
  template: `
    <div class="mdl-card mdl-shadow--4dp settings mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">Settings</h2>
      </div>

      <div class="mdl-card__supporting-text">

        <h5>Export Mask</h5>

        <button v-on:click="exportMasks()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Export masks
        </button> 

        <h5>Export Data</h5>

        <button v-on:click="exportData()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Export data
        </button> 

        <h5>Import Masks</h5>

        <input type="file" ref="importMaskFile">

        <button v-on:click="importMaskCsv()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Import Mask CSV
        </button> 

        <h5>Import Data</h5>

        <input type="file" ref="importDataFile">

        <button v-on:click="importDataCsv()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Import Data
        </button> 

        <h5>Serial Communication</h5>
        <button v-on:click="getPorts" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Get Ports
        </button>

        <table class="mdl-data-table mdl-js-data-table mdl-shadow--2dp ports-table">
          <thead>
            <tr>
              <th>Comm Name</th>
              <th>Manufacturer</th>
              <th>Set</th>
            </tr>
          </thead>

          <tbody>
            <tr v-for="port in ports">
              <td>{{ port.comName }}</td>
              <td>{{ port.manufacturer }}</td>
              <td>
                <button v-on:click="setPort(port)" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">Set</button>
              </td>
            </tr>
          </tbody>
        </table>

        <button v-on:click="disconnect" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Disconnect Serial
        </button>

      </div>
    </div>
  `,

  props: ['bus', 'data'],

  data() {
    return {
      ports: [],
      voltageRange: [-12, 12],
      currentRange: [-200, 200],
      resistanceRange: [0, 1000000],
      logicRange: [-0.5, 1.5],

      overlimitIdentifier: 'OL',
    };
  },

  methods: {

    /**
     * Polls the server for a list of connected serial ports
     */
    getPorts() {
      this.bus.$emit('get-ports');
    },

    checkOverlimit(measurement, y) {
      switch (measurement) {
        case 'voltage':
          if (y < this.voltageRange[0] || y > this.voltageRange[1]) {
            return this.overlimitIdentifier;
          }
          break;
        case 'current':
          if (y < this.currentRange[0] || y > this.currentRange[1]) {
            return this.overlimitIdentifier;
          }
          break;
        case 'resistance':
          if (y < this.resistanceRange[0] || y > this.resistanceRange[1]) {
            return this.overlimitIdentifier;
          }
          break;
        default:
          break;
      }

      // Return the original value for the measurement
      return y;
    },

    createMasksCsv(masks) {
      let meas = null;

      switch (masks.measurement) {
        case 'voltage':
          meas = 'V';
          break;
        case 'current':
          meas = 'A';
          break;
        case 'resistance':
          meas = 'Ohm';
          break;
        default:
          meas = 'N/A';
          break;
      }

      let csv = masks.topMasks.map((m) => {
        return `${m.mask},${m.x},${m.y},${meas}`;
      }).join('\n');

      csv += '\n';

      csv += masks.bottomMasks.map((m) => {
        return `${m.mask},${m.x},${m.y},${meas}`;
      }).join('\n');

      const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
      saveAs(blob, 'data.csv');
    },

    /**
     * Export masks
     */
    exportMasks() {
      this.bus.$emit('get-masks');
    },

    /**
     * Export data
     */
    exportData() {
      const csv = this.data.map((d) => {
        const date = d.date.toISOString();
        const currentMode = d.currentMode;
        let meas = '';
        switch (currentMode) {
          case 'voltage':
            meas = 'V';
            break;
          case 'current':
            meas = 'A';
            break;
          case 'resistance':
            meas = 'R';
            break;
          default:
            meas = 'N/A';
            break;
        }
        const val = this.checkOverlimit(currentMode, d.value);
        return `${date},${val},${meas}`; }).join('\n');

      const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
      saveAs(blob, 'data.csv');
    },


    /**
     * Mask CSV files
     */
    importMaskCsv() {
      if (this.$refs.importMaskFile.files.length < 1) {
        this.bus.$emit('show-snackbar', 'No file selected');
        return;
      }

      Papa.parse(this.$refs.importMaskFile.files[0], {
        complete: this.maskCallback,
      });
    },

    /**
     * The callback for the papa parse function
     */
    maskCallback(results) {
      const parsed = results.data.map(this.parseMaskLine).filter(n => n != null);

      const topMasks = [];
      const bottomMasks = [];

      parsed.forEach((mask) => {
        if (mask.mask === 'high') {
          topMasks.push(mask);
        } else if (mask.mask === 'low') {
          bottomMasks.push(mask);
        } else {
          console.log(`Tried to parse mask '${mask.mask}'`);
        }
      });
      const collisions = collision.checkCollision([], topMasks, bottomMasks);

      if (collisions.length === 0) {
        this.bus.$emit('set-masks', parsed);
      } else {
        alert('Invalid mask file!');
      }
    },

    parseMaskLine(line) {
      // Check for line errors
      if (line.length !== 4) {
        return null;
      }

      const parsed = {
        mask: line[0],
        x: parseFloat(line[1]),
        y: parseFloat(line[2]),
        unit: line[3],
      };

      return parsed;
    },

    /**
     * Data CSV files
     */
    importDataCsv() {
      Papa.parse(this.$refs.importDataFile.files[0], {
        complete: this.dataCallback,
      });
    },

    /**
     * The callback for the papa parse function
     */
    dataCallback(results) {
      const parsed = results.data.map(this.parseDataLine).filter(n => n != null);

      this.bus.$emit('set-data', parsed);
    },

    parseDataLine(line) {
      // Check for line errors
      if (line.length !== 3) {
        return null;
      }
      let meas = '';

      switch (line[2]) {
        case 'V':
          meas = 'voltage';
          break;
        case 'A':
          meas = 'current';
          break;
        case 'R':
          meas = 'resistance';
          break;
        default:
          meas = 'N/A';
          break;
      }

      const parsed = {
        date: moment(line[0]),
        value: parseFloat(line[1]),
        currentMode: meas,
      };

      return parsed;
    },

    loadPorts(ports) {
      this.ports = ports;
    },

    /**
     * Sets the port to connect to
     */
    setPort(port) {
      this.bus.$emit('setport', port);
    },

    /**
     * Disconnects the serial devices
     */
    disconnect() {
      this.bus.$emit('device-disconnect');
    },
  },

  mounted() {
    this.bus.$on('portslist', this.loadPorts);
    this.bus.$on('masks', this.createMasksCsv);
  },
});
