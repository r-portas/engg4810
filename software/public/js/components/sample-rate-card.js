/* global Vue */

Vue.component('sample-rate-card', {
  template: `
    <div class="mdl-card mdl-shadow--4dp mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          Sample Rate
        </h2>
      </div>

      <div class="mdl-card__supporting-text">

        <select v-model="sampleRate">
          <option value="2">2 measurements per second</option>
          <option value="1">1 measurement per second</option>
          <option value="0.5">1 measurement every 2 seconds</option>
          <option value="0.2">1 measurement every 5 seconds</option>
          <option value="0.1">1 measurement every 10 seconds</option>
          <option value="0.016">1 measurement every minute</option>
          <option value="0.0083">1 measurement every 2 minutes</option>
          <option value="0.0033">1 measurement every 5 minutes</option>
          <option value="0.0016">1 measurement ever 10 minutes</option>
        </select>

        <button v-on:click="setRate" class="log-btn mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Set Sampling Rate
        </button>
        
      </div>
    </div>
  `,

  data() {
    return {
      sampleRate: 1,
    };
  },

  props: ['bus'],

  methods: {
    /**
     * Set the sampling rate in hertz
     */
    setRate() {
      const sampleRateString = `s ${this.sampleRate}`;
      this.bus.$emit('sampling-rate', sampleRateString);
    },
  },

  created() {
  },
});
