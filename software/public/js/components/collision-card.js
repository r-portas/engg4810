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
          <li v-for="collision in topCollisions" v-bind:class="{ 'invalid-mask': checkInvalid(collision) }" class="mdl-list__item">
            {{collision[0]}}
          </li>

          <li v-for="collision in bottomCollisions" v-bind:class="{ 'invalid-mask': checkInvalid(collision) }" class="mdl-list__item">
            {{collision[0]}}
          </li>
        </ul>
      </div>
    </div>
  `,

  data() {
    return {
      topCollisions: [],
      bottomCollisions: [],
    };
  },

  props: ['bus'],

  methods: {
    checkTopCollisions(data) {
      console.log(data);
      this.topCollisions = data;
    },

    checkBottomCollisions(data) {
      console.log(data);
      this.bottomCollisions = data;
    },

    checkInvalid(collision) {
      return collision[0] === 'Invalid Mask Configuration';
    },

    /**
     * Processes the collisions from the checkCollision function
     */
    processCollisions(collisions) {
      for (let i = 0; i < collisions.length; i += 1) {
        console.log(collisions[i]);
      }
    },
  },

  created() {
    this.bus.$on('check-top-collisions', this.checkTopCollisions);
    this.bus.$on('check-bottom-collisions', this.checkBottomCollisions);
  },
});
