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
 
 if (!$('.toggle-date-range-filter').hasClass('processed')) {
   
   // hide all regions that should be collapsed
   $('.date-range-collapsed').next('.date-range-filter-wrapper').css({'display': 'none'});
   
   $('.toggle-date-range-filter').click(function() {
     $(this).next('.date-range-filter-wrapper').slideToggle('fast');
     
     return false;
   });

   $('.toggle-date-range-filter').addClass('processed');
 }
}