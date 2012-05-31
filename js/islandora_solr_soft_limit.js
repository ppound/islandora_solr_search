/**
 * @file
 * Snippet for facet block toggle to expose and hide facet fields.
 */

(function ($) {

  Drupal.behaviors.islandora_solr_toggle = {
    attach: function(context, settings) {
      // show more
      if (!$(".soft-limit").hasClass('processed')) {
        $(".soft-limit").click(function(e) {
          // toggle class .hidden
          $(this).siblings(".hidden, .toggle-wrapper").toggleClass('hidden');
          $(this).toggleClass('hidden');

          e.preventDefault();
        });
        $(".soft-limit").addClass('processed');
      }
    }
  };

})(jQuery);