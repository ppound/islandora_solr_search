/* Script for islandora_solr admin */
(function ($) {
console.log('js');
  // function to trigger a form buttom when clicking on a link element.
  Drupal.behaviors.islandora_solr_remove = {
    attach: function(context, settings) {
      // show more
      $(".islandora-solr-remove-link").each(function () {

        if (!$(this).hasClass('processed')) {
          $('.islandora-solr-remove-link').click(function() {
            $(this).parent('.islandora-solr-operations').next('td').find('input').trigger('click');
            return false;
          });
          $(".islandora-solr-remove-link").addClass('processed');
        }

      });
      
    }
  };

})(jQuery);