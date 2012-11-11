/* Script for islandora_solr admin */
(function ($) {

  // function to trigger a form button when clicking on a link element.
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
            $(this).parent('.islandora-solr-operations').next('td').find('input').trigger('click');
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
    }
  };
  
  // Add dialog form values to the drupal ajax settings
  // @see http://drupal.org/node/1028410#comment-4301262
  // @TODO: check if this might be a better approach: http://drupal.stackexchange.com/questions/9920/how-to-extend-or-hook-drupal-form-ajax/10191#10191
  $.fn.islandoraSolrDialogValues = function(data) {
    // dialog ajax id
    var dialogAjaxId = data.id;
    // add values as JSON, so we can pass multi dimensional arrays
    Drupal.ajax[dialogAjaxId].options.data._dialog_values = JSON.stringify(data.values);
	};
  
})(jQuery);