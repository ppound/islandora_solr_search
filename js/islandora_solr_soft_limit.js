/**
 * @file
 * Snippet for facet block toggle to expose and hide facet fields.
 */

(function ($) {

  // Adds facet toggle functionality
  Drupal.behaviors.islandoraSolrToggle = {
    attach: function(context, settings) {
      // show more
      if (!$(".soft-limit").hasClass('processed')) {
        $(".soft-limit").click(function(e) {
          // toggle class .hidden
          $(this).prev(".islandora-solr-facet").toggleClass('hidden');
          if ($(this).text() == Drupal.t('Show more')) {
            $(this).text(Drupal.t('Show less'));
          }
          else {
            $(this).text(Drupal.t('Show more'));
          }
          e.preventDefault();
        });
        $(".soft-limit").addClass('processed');
      }
    }
  };

})(jQuery);