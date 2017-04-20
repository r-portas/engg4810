/* global Vue */

Vue.component('data-view', {
  template: `
    <div class="mdl-card mdl-shadow--4dp mm-card settings">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          Data View
        </h2>
      </div>

      <div class="mdl-card__supporting-text">
        <ul class="mdl-list">
          <li v-for="item in data" class="mdl-list__item">
            {{item.date}}: {{item.value}}
          </li>
        </ul>
      </div>
    </div>
  `,

  data() {
    return {
    };
  },

  props: ['data'],

  methods: {
  },
});
