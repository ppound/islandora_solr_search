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

  if (rangeSliderVals) {
    // loop over each range slider facet
    $.each(rangeSliderVals, function() {
      // set variables
      var sliderData = this.data;
      var form_key = this.form_key;
      var sliderId = '#date-range-slider-' + form_key;
      var amountId = '#slider-amount-' + form_key;
      var canvasId = '#date-range-slider-canvas-' + form_key;

      var sliderMax = sliderData.length - 1;
      var sliderMin = 0;
      var sliderStep = 1;

      // add jquery ui slider
      $(sliderId).slider({
        range: true,
        handles: [{start:sliderMin, min:sliderMin, max:sliderMax, id:'range-slider-handle-min-' + form_key}, {start:sliderMax, min:sliderMin, max:sliderMax, id:'range-slider-handle-max-' + form_key}],
        values: [sliderMin, sliderMax],
        min: sliderMin,
        max: sliderMax,
        step: sliderStep,
        slide: function(event, ui) {
          sliderUpdate(ui);
        },
        slide: function(event, ui) {
          sliderUpdate(ui);
        }
      });

      function sliderUpdate(ui) {

        // get values
        var fromVal = ui.values[0];
        var toVal = ui.values[1];

        // get dates
        var fromDate = sliderData[fromVal].date;
        var toDate = sliderData[toVal].date;

        // assign to hidden field
        $('.range-slider-hidden-from-' + form_key).val(fromDate);
        $('.range-slider-hidden-to-' + form_key).val(toDate);

        // get formatted dates
        var formatFromDate = sliderData[fromVal].format_date;
        var formatToDate = sliderData[toVal].format_date;

        // assign to popup
        $(sliderId + ' .slider-popup-from').html(formatFromDate);
        $(sliderId + ' .slider-popup-to').html(formatToDate);

        // update plots
        plot.unhighlight();
        for (i = fromVal; i < toVal; i++) {
          plot.highlight(0, i);
        }
      }

      // set canvas width equal to slider width
      var canvasWidth = $(sliderId).width();
      $(canvasId).width(canvasWidth - 0).height('120px');

      // add classes to slider handles
      $(sliderId + ' > a:eq(0)').addClass('handle-min').prepend('<div class="slider-popup-from">' + sliderData[0].format_date + '</div>').hover(function() {
        $(this).find('.slider-popup-from').stop(false, true).fadeIn(0);
      }, function() {
        $(this).find('.slider-popup-from').stop(false, true).fadeOut('slow');
      });

      $(sliderId + ' > a:eq(1)').addClass('handle-max').prepend('<div class="slider-popup-to">' + sliderData[sliderData.length-1].format_date + '</div>').hover(function() {
        $(this).find('.slider-popup-to').stop(false, true).fadeIn(0);
      }, function() {
        $(this).find('.slider-popup-to').stop(false, true).fadeOut('slow');
      });




      // Flot
      // prepare flot data
      var d1 = [];
      for (var i = 0; i <= sliderMax - 1; i += 1) {
        d1.push([i, this.data[i].bucket_size]);
      }

      // render Flot graph
      var plot = $.plot($(canvasId), [d1], {
        colors: ["#edc240", "#afd8f8", "#cb4b4b", "#4da74d", "#9440ed"],
        xaxis: {  ticks: [], min: 0, autoscaleMargin: 0},
        yaxis: {  ticks: [], min: 0, autoscaleMargin: 0},
        series: {
          stack: false,
          lines: {
            show: false
          },
          bars: {
            show: true,
            lineWidth: 1, // in pixels
            barWidth: 0.8, // in units of the x axis
            fill: true,
            fillColor: null,
            align: "left", // or "center" 
            horizontal: false
          }
        },
        grid: {
          show: true,
          labelMargin: null, // in pixels
          axisMargin: null, // in pixels
          borderWidth: null, // in pixels
          markingsLineWidth: null,
          // interactive stuff
          clickable: true,
          hoverable: false,
          autoHighlight: true, // highlight in case mouse is near
          mouseActiveRadius: 10 // how far the mouse can be away to activate an item
        }
      });


      // add plotclick event to update the sliders
      $(canvasId).bind("plotclick", function (event, pos, item) {
        if (item !== null) {
          var dataIndexValue = item.dataIndex;
          plot.unhighlight();
          $(sliderId).slider('values', 0, dataIndexValue);
          $(sliderId).slider('values', 1, dataIndexValue + 1);
          plot.highlight(item.series, item.datapoint);
        }
      });

    }); // end $.each()
  }
}

