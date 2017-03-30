/* global Vue */

Vue.component('collision-card', {
  template: `
    <div class="mdl-card mdl-shadow--4dp mm-card">
      <div class="mdl-card__title">
        <h2 class="mdl-card__title-text">
          Collisions
        </h2>
      </div>

      <div class="mdl-card__supporting-text">
        <ul class="mdl-list">
          <li v-for="collision in collisions" v-bind:class="{ 'invalid-mask': checkInvalid(collision) }" class="mdl-list__item">
            {{collision}}
          </li>
        </ul>
      </div>
    </div>
  `,

  data() {
    return {
      collisions: [],
    };
  },

  props: ['bus'],

  methods: {
    handleCollisions(collisions) {
      this.collisions = collisions;
    },

    checkInvalid(collision) {
      return collision.name === 'Invalid Mask Configuration';
    },
  },

  created() {
    this.bus.$on('collisions', this.handleCollisions);
  },
});
