/* global Vue, Papa */

Vue.component('settings', {
  template: `
    <div class="mdl-card settings">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">Settings</h2>
      </div>

      <div class="mdl-card__supporting-text">

        <input type="file" ref="importFile">

        <button v-on:click="importCsv()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Import CSV
        </button> 

      </div>
    </div>
  `,

  methods: {
    importCsv() {
      Papa.parse(this.$refs.importFile.files[0], {
        complete: this.complete,
      });
    },

    /**
     * The callback for the papa parse function
     */
    complete(results) {
      const clean = results.data.map(this.parseLine);
      console.log(clean);
    },

    parseLine(line) {
      // Check for line errors
      if (line.length !== 4) {
        return null;
      }

      const parsed = {};

      console.log(line);
      return line;
    },
  },
});
