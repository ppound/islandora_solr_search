
(function ($) {

  Drupal.behaviors.islandora_solr_remove = {
    attach: function(context, settings) {
      // show more
      if (!$(".islandora-solr-remove-link").hasClass('processed')) {
        $('.islandora-solr-remove-link').click(function() {
          $(this).parent('.islandora-solr-operations').next('td').find('input').trigger('click');
          return false;
        });
        $(".islandora-solr-remove-link").addClass('processed');
      }
    }
  };

})(jQuery);