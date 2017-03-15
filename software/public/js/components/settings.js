/* global Vue, Papa */

Vue.component('settings', {
  template: `
    <div class="mdl-card settings">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">Settings</h2>
      </div>

      <div class="mdl-card__supporting-text">

        <input type="file" ref="importFile">

        <button c-on:click="importCsv()" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Import CSV
        </button> 

      </div>
    </div>
  `,

  methods: {
    importCsv() {
      console.log(Papa.parse(this.$refs.importFile.files[0]));
    },
  },
});
