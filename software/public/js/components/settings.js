/* global Vue, Papa, moment, Blob, saveAs */

Vue.component('settings', {
  template: `
    <div class="mdl-card mdl-shadow--4dp settings mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">Settings</h2>
      </div>

      <div class="mdl-card__supporting-text">

        <h5>Export Mask</h5>

        <button v-on:click="exportMasks()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Export data
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

      </div>
    </div>
  `,

  props: ['bus', 'data'],

  data() {
    return {
      ports: []
    };
  },

  methods: {

    /**
     * Polls the server for a list of connected serial ports
     */
    getPorts() {
      this.bus.$emit('getports');
    },

    /**
     * Export masks
     */
    exportMasks() {
      const csv = this.data.map((d) => { return ''; }).join('\n');

      const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
      saveAs(blob, 'data.csv');
    },

    /**
     * Export data
     */
    exportData() {
      const csv = this.data.map((d) => {
        const date = d.date.toISOString();
        return `${date},${d.value}`;
      }).join('\n');

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

      this.bus.$emit('set-masks', parsed);
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
      if (line.length !== 2) {
        return null;
      }

      const parsed = {
        date: moment(line[0]),
        value: parseFloat(line[1]),
      };

      return parsed;
    },

    loadPorts(ports) {
      this.ports = ports;
    },

    setPort(port) {
      console.log(port);
    },
  },

  mounted() {
    this.bus.$on('portslist', this.loadPorts);
  },
});
