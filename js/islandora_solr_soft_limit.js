$(document).ready(function() {
  
  // show more  
  $(".soft-limit").click(function() {
  
    $(this).siblings(".hidden, .toggle-wrapper").toggleClass('hidden');
    $(this).toggleClass('hidden');

    return false;
   });

});