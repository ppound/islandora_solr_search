/* Script for islandora_solr admin */
(function ($) {

  // function to trigger a form buttom when clicking on a link element.
  Drupal.behaviors.islandoraSolrRemove = {
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

// function to trigger a form buttom when clicking on a link element.
  Drupal.behaviors.islandoraSolrConfigure = {
    attach: function(context, settings) {
      // show more
      $(".islandora-solr-configure-link").each(function () {

        if (!$(this).hasClass('processed')) {
          $('.islandora-solr-configure-link').click(function() {
//            $(this).parent('.islandora-solr-operations').next('td').find('input').trigger('click');
$('#islandora-solr-admin-dialog').dialog('open');
            return false;
          });
          $(".islandora-solr-configure-link").addClass('processed');
        }

      });
      
    }
  };

  // function for the  dialog box
  Drupal.behaviors.islandoraSolrDialog = {
    attach: function(context, settings) {
      
//      if (!settings.views) {
//        return;
//      }
      // Create a jQuery UI dialog, but leave it closed.
      var dialog_area = $('#islandora-solr-admin-dialog', context);
      dialog_area.dialog({
        'autoOpen': false,
        'dialogClass': 'islandora-solr-admin-dialog',
        'modal': true,
        'position': 'center',
        'resizable': false,
        'width': 750,
        'draggable': false
      });
      
      
//      $(".example").each(function () {
//
//        if (!$(this).hasClass('processed')) {
//          // stuff
//          
//          $(".example").addClass('processed');
//        }
//      });
    }
  };
  
})(jQuery);
