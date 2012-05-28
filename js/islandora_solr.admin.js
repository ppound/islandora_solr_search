/* JS file for islandora_solr admin interface */

(function($) {
  // Expands textarea when enabled
  Drupal.behaviors.islandora_solr_admin_sortby = {
    attach: function (context, settings) {
      // set variables: checkbox and textarea wrapper
      var checkbox = $('input#edit-islandora-solr-search-sortby');
      var wrapper = $('.form-item-islandora-solr-search-sortby-terms');
      // add dependent class
      wrapper.addClass('dependent-options');
console.log("loaded");
      // set default state
      if (!checkbox.is(':checked')) {
        wrapper.hide();
      }

      // on change hide/show textarea wrapper
      checkbox.change(function() {
        if (!checkbox.is(':checked')) {
          wrapper.hide();
        }
        else {
          wrapper.show();
        }
      });
    }
  };
}(jQuery));