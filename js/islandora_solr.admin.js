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
        'width': 900,
        'height': 500,
        'draggable': false
      });
    }
  };
  
  // function for the dialog box window resize event
  Drupal.behaviors.islandoraSolrDialogResize = {
    attach: function(context, settings) {
      // resize dialog box
            
      // @TODO: lots of small bugs in here.
            
      var resizeModal = function() {
        // calculate dimensions
        // dom elements
        var $modal = $('#islandora-solr-admin-dialog', context);
        var $scroll = $('#islandora-solr-admin-dialog-form', context);
        // window
        var windowWidth = $(window).width();
        var windowHeight = $(window).height();
        // max modal values
        var maxWidth = parseInt(windowWidth * .6); // 60%
        var maxHeight = parseInt(windowHeight * .8); // 80%
        // get scroll height
        var scrollHeight = $scroll.height();
        // get modal height
        var modalHeight = $modal.height();
        // get modal height
        var modalHeightCalc = scrollHeight + 98; // scroll + header & footer
        // if maximum height is larger than the calculated height, we use the calculated height
        if (maxHeight > modalHeightCalc) {
        
        }
        else { // else we use the maximum height
          // @TODO, when fieldset is larger than $scroll, this gets set correctly and stays that way. When fieldset isn't quite that big and doesn't get this assigned, collapsing and expanding again, will cause the dialog to jump, because no fixed height is set.
          $scroll.css({
            'height': scrollHeight + 'px', // 1) 494 2) 465
            'max-height': scrollHeight + 'px'
          });
         
        }
        // set dialogHeight
        var dialogHeight = modalHeightCalc;
        if (dialogHeight > maxHeight) {
          dialogHeight = maxHeight;
          $scroll.css({
            // @TODO: when the max height is reached and window is made smaller, this will indeed make the $scroll height smaller, but when stretching out again, it doesn't gain size.
            'height': (dialogHeight - 98) + 'px', // 1) 494 2) 465
            'max-height': (dialogHeight - 98) + 'px'
          });
        }
        // apply dimensions
        $modal.dialog('option', 'width', maxWidth);
        $modal.dialog('option', 'height', dialogHeight);
        $modal.dialog('option', 'position', 'center');        
      }
      // apply dialog dimensions on load
      resizeModal();
      // apply dialog dimensions on resize
      $(window).resize(function() {
        resizeModal();
      });

      // on fieldset collapse event
      // snippet from views ui, which has taken part of a snippet from collapse.js. Yo dawg..
      if (!this.collapseReplaced && Drupal.collapseScrollIntoView) {
        this.collapseReplaced = true;
        Drupal.collapseScrollIntoView = function (node) {
          for (var $parent = $(node); $parent.get(0) != document && $parent.size() != 0; $parent = $parent.parent()) {
            if ($parent.css('overflow') == 'scroll' || $parent.css('overflow') == 'auto') {

                // If the modal is already at the max height, don't bother with
                // this since the only reason to do it is to grow the modal.
                if ($('.views-ui-dialog').height() < parseInt($(window).height() * .8)) {
                  resizeModal();
                }
              return;
            }
          }
          var h = document.documentElement.clientHeight || document.body.clientHeight || 0;
          var offset = document.documentElement.scrollTop || document.body.scrollTop || 0;
          var posY = $(node).offset().top;
          var fudge = 55;
          if (posY + node.offsetHeight + fudge > h + offset) {
            if (node.offsetHeight > h) {
              window.scrollTo(0, posY);
            }
            else {
              window.scrollTo(0, posY + node.offsetHeight - h + fudge);
            }
          }
        };
      }
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

  // attach behaviors to make sure all javascript is added to the form in the dialog	
	$.fn.islandoraSolrAttachBehaviors = function() {
    Drupal.attachBehaviors();
	};
  
})(jQuery);