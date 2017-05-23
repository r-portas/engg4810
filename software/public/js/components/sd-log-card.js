/* global Vue */

Vue.component('sd-log-card', {
  template: `
    <div class="mdl-card mdl-shadow--4dp mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          SD Card Logging
        </h2>
      </div>

      <div class="mdl-card__supporting-text">

        <div class="mdl-textfield mdl-js-textfield mdl-textfield--floating-label">
          <input v-model="sampleNum" class="mdl-textfield__input" type="text" id="sampleNum">
          <label class="mdl-textfield__label" for="sampleNum">Number of samples</label>
        </div>

        <div class="mdl-textfield mdl-js-textfield mdl-textfield--floating-label">
          <input v-model="sampleRate" class="mdl-textfield__input" type="text" id="sampleRate">
          <label class="mdl-textfield__label" for="sampleRate">Sample rate</label>
        </div>

        <button :disabled="isLogging" v-on:click="startLog" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Start Logging 
        </button>

        <button :disabled="!isLogging" v-on:click="stopLog" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Stop Logging 
        </button>
        
      </div>
    </div>
  `,

  data() {
    return {
      isLogging: false,
      sampleRate: 0,
      sampleNum: 0,
    };
  },

  props: ['bus'],

  methods: {
    startLog() {
      this.isLogging = true;

      this.bus.$emit('start-log', { sampleRate: this.sampleRate, sampleNum: this.sampleNum });
    },

    stopLog() {
      this.isLogging = false;
    },
  },

  created() {
    this.bus.$on('logging-finished', () => this.stopLog());
  },
});
