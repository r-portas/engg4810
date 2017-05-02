/* global Vue */

Vue.component('delete-dialog', {
  template: `
    <div>

      <!-- A button to show the modal -->
      <button v-on:click="openModal" type="button" class="mdl-button mdl-js-button mdl-button--raised mdl-button--colored">Delete data</button>

      <dialog class="mdl-dialog" ref="dialog">
        <div class="mdl-dialog__content">
          <p>
            Delete stored data from the application
          </p>
        </div>
        <div class="mdl-dialog__actions">
          <button v-on:click="deleteData" type="button" class="mdl-button">Delete</button>
          <button v-on:click="closeModal" type="button" class="mdl-button">Cancel</button>
        </div>
      </dialog>
    </div>
    `,

  data() {
    return {
      dialog: null,
    };
  },

  props: ['bus'],

  methods: {
    openModal() {
      this.dialog.showModal();
    },

    closeModal() {
      this.dialog.close();
    },

    deleteData() {
      this.bus.$emit('delete-data');
      this.dialog.close();
    },
  },

  mounted() {
    this.dialog = this.$refs.dialog;
  },
});
