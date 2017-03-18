/* global Vue */

Vue.component('data-card', {
  template: `
    <div class="mdl-card mdl-shadow--4dp">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          Data
        </h2>
      </div>

      <div class="mdl-card__supporting-text">
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

      </div>
    </div>
  `,

  data() {
    return {
      rangeOption: 'auto',
    };
  },

  props: ['bus'],

  methods: {
  },

  watch: {
    rangeOption(newVal) {
      this.bus.$emit('set-range', newVal);
    },
  },
});
