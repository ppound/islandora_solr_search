/* Javascript file for islandora solr search facets */

// Show facet fields after soft limit
Drupal.behaviors.shownToggle = function (context) {
  
  if (!$('.shown-toggle').hasClass('processed')) {
   // show more  
   $('.shown-toggle').click(function() {
  
     $(this).siblings(".hidden, .toggle-wrapper").toggleClass('hidden');
     $(this).toggleClass('hidden');

     return false;
    });
    
    $('.shown-toggle').addClass('processed');
  }
};


// Show/hide date range filter
Drupal.behaviors.dateRangeFilter = function (context) {
 
  // set variables
  var stringHide = Drupal.t('Hide');
  var stringShow = Drupal.t('Show');
 
  if (!$('.toggle-date-range-filter').hasClass('processed')) {
   
    // hide all regions that should be collapsed
    $('.date-range-collapsed').next('.date-range-filter-wrapper').css({'display': 'none'});
   
    $('.toggle-date-range-filter').click(function() {
      // toggle strings
      if ($(this).html() == stringHide) {
        $(this).html(stringShow);
      }
      else {
        $(this).html(stringHide);
      }
   
      // toggle wrapper
      $(this).next('.date-range-filter-wrapper').slideToggle('fast');
     
      return false;
    });

    $('.toggle-date-range-filter').addClass('processed');
  }
}


// Datepicker
Drupal.behaviors.dateRangeFilterDatepicker = function (context) {

  // get year range variable
  var yearRangeVal = Drupal.settings.islandora_solr_search_datepicker_range;

  // set datepicker
  // @TODO: add date range functionality: http://jqueryui.com/demos/datepicker/#date-range
  $(".islandora-solr-search-datepicker").datepicker({
    changeMonth: true,
    changeYear: true,
    dateFormat: "yy/mm/dd",
    yearRange: yearRangeVal
  });
}

