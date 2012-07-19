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



// Range slider
Drupal.behaviors.dateRangeSlider = function (context) {

  // get year range variable
  var rangeSliderVals = Drupal.settings.islandora_solr_search_range_slider;

  
  $.each(rangeSliderVals, function() {
    var sliderData = this.data;
    var form_key = this.form_key;
    var sliderId = '#date-range-slider-' + form_key;
    var amountId = '#slider-amount-' + form_key;
    var canvasId = '#date-range-slider-canvas-' + form_key;
    
    var sliderMax = sliderData.length - 1;
    var sliderMin = 0;
    var sliderStep = 1;


    $(sliderId).slider({
      range: true,
      handles: [{start:sliderMin, min:sliderMin, max:sliderMax, id:'range-slider-handle-min-' + form_key}, {start:sliderMax, min:sliderMin, max:sliderMax, id:'range-slider-handle-max-' + form_key}],
      values: [sliderMin, sliderMax],
      min: sliderMin,
      max: sliderMax,
      stepping: sliderStep, // this is named 'step' in newer versions of jquery ui
      change: function(event, ui) {
        // assign values to form fields. // @TODO: this is done differently in newer versions of jquery ui        
        // min handler: assign to 'from' field
        if (ui.handle[0].id == 'range-slider-handle-min-' + form_key) {
          var fromDate = sliderData[ui.value].date;
          $('.range-slider-hidden-from-' + form_key).val(fromDate);
        }
        // max handler: assign to 'to' field
        if (ui.handle[0].id == 'range-slider-handle-max-' + form_key) {
          var toDate = sliderData[ui.value].date;
          $('.range-slider-hidden-to-' + form_key).val(toDate);
        }
console.log(ui.value);
//        $(amountId).html(ui.value);
      }
    });

    var canvasWidth = $(sliderId).width();
    console.log(canvasWidth);
    $(canvasId).width(canvasWidth);

//    $('.range-slider-hidden-to-' + form_key).val(toDate);
//    $(amountId).html($(sliderId).slider("value"));
  });
  
  
}

