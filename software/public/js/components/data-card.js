/* global Vue */

Vue.component('data-card', {
  template: `
    <div class="mdl-card mdl-shadow--4dp mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          Data
        </h2>
      </div>

      <p>Current mode: <b>{{rangeOption}}</b></p>
      
      <p>LCD:</p>
  
      <p v-html="lcdText" style="font-family: monospace;"></p>

      <p>LCD Brightness Control</p>

      <input class="mdl-slider mdl-js-slider" type="range"
          min="0" :max="5" v-model="lcdBrightness" tabindex="0">

      <div class="mdl-card__supporting-text">
        <button v-on:click="togglePause" class="settings-btn mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
          Toggle Pause
        </button> 
        <br>

        <label class="mdl-switch mdl-js-switch mdl-js-ripple-effect" for="ac-switch">
          <input v-model="acSwitch" type="checkbox" id="ac-switch" class="mdl-switch__input">
          <span class="mdl-switch__label">AC Mode</span>
        </label>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="auto">
          <input type="radio" id="auto" class="mdl-radio__button" value="auto" v-model="rangeOption">
          <span class="mdl-radio__label">Auto</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="voltage">
          <input type="radio" id="voltage" class="mdl-radio__button" value="voltage" v-model="rangeOption" v-on:click="setRange('voltage')">
          <span class="mdl-radio__label">Voltage</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="current">
          <input type="radio" id="current" class="mdl-radio__button" value="current" v-model="rangeOption" v-on:click="setRange('current')">
          <span class="mdl-radio__label">Current</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="resistance">
          <input type="radio" id="resistance" class="mdl-radio__button" value="resistance" v-model="rangeOption" v-on:click="setRange('resistance')">
          <span class="mdl-radio__label">Resistance</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="logic">
          <input type="radio" id="logic" class="mdl-radio__button" value="logic" v-model="rangeOption" v-on:click="setRange('logic')">
          <span class="mdl-radio__label">Logic Level</span>
        </label> 
        <br>

        <label class="mdl-radio mdl-js-radio mdl-js-ripple-effect" for="continuity">
          <input type="radio" id="continuity" class="mdl-radio__button" value="continuity" v-model="rangeOption" v-on:click="setRange('continuity')">
          <span class="mdl-radio__label">Continuity</span>
        </label> 
        <br>

        <button v-on:click="clearMasks" class="settings-btn mdl-button mdl-js-button mdl-button--raised mdl-button--colored">
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
      lcdText: '',
      lcdBrightness: 0,
      acSwitch: false,
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

    setRange(range) {
      this.bus.$emit('set-range', range);
    },
  },

  watch: {
    randomSwitch(newVal) {
      if (newVal) {
        this.startRandomData();
      } else {
        this.stopRandomData();
      }
    },

    /**
     * AC switch update
     */
    acSwitch(newVal) {
      if (newVal === true) {
        this.bus.$emit('set-ac-mode', 'a 1');
      } else {
        this.bus.$emit('set-ac-mode', 'a 0');
      }
    },

    /**
     * Sets the lcd brightness
     */
    lcdBrightness(newVal) {
      this.bus.$emit('set-brightness', `b ${newVal}`);
    },
  },

  mounted() {
    this.bus.$on('set-range', (newRange) => {
      this.rangeOption = newRange;
      console.log(`Updating range: ${this.rangeOption}`);
    });

    this.bus.$on('lcd-text', (text) => {
      this.lcdText = text.replace(/^\$ /g, '');
    });

    this.bus.$on('update-ac-mode', (val) => {
      this.acSwitch = val;
    });
  },
});
