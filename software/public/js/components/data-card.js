/* global Vue */

Vue.component('data-card', {
  template: `
    <div class="mdl-card mdl-shadow--4dp mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          Data
        </h2>
      </div>

      <div class="mdl-card__supporting-text">
        <button v-on:click="togglePause" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Toggle Pause
        </button> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="auto">
          <input type="radio" id="auto" class="mdl-radio__button" value="auto" v-model="rangeOption">
          <span class="mdl-radio__label">Auto</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="voltage">
          <input type="radio" id="voltage" class="mdl-radio__button" value="voltage" v-model="rangeOption">
          <span class="mdl-radio__label">Voltage</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="current">
          <input type="radio" id="current" class="mdl-radio__button" value="current" v-model="rangeOption">
          <span class="mdl-radio__label">Current</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="resistance">
          <input type="radio" id="resistance" class="mdl-radio__button" value="resistance" v-model="rangeOption">
          <span class="mdl-radio__label">Resistance</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="logic">
          <input type="radio" id="logic" class="mdl-radio__button" value="logic" v-model="rangeOption">
          <span class="mdl-radio__label">Logic Level</span>
        </label> 
        <br>

        <button v-on:click="clearMasks" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Clear Masks
        </button> 

        <label class="mdl-switch mdl-js-switch mdl-js-ripple-effect" for="random-switch">
          <input v-model="randomSwitch" type="checkbox" id="random-switch" class="mdl-switch__input">
          <span class="mdl-switch__label">Random data</span>
        </label>

        <delete-dialog :bus="bus"></delete-dialog>
      </div>
    </div>
  `,

  data() {
    return {
      rangeOption: 'auto',
      randomSwitch: false,
    };
  },

  props: ['bus'],

  methods: {
    clearMasks() {
      this.bus.$emit('set-masks', []);
    },

    startRandomData() {
      this.bus.$emit('start-random-data');
    },

    stopRandomData() {
      this.bus.$emit('stop-random-data');
    },

    togglePause() {
      this.bus.$emit('toggle-pause');
    },
  },

  watch: {
    rangeOption(newVal) {
      this.bus.$emit('set-range', newVal);
    },

    randomSwitch(newVal) {
      if (newVal) {
        this.startRandomData();
      } else {
        this.stopRandomData();
      }
    },
  },
});
