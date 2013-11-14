window.onload = function() {
  var locs = Drupal.settings.islandora_solr_geo.locations;
  var latlngbounds = new google.maps.LatLngBounds();
  var marker_locations = new Array();

  if(locs.length > 1) {
    for(var i = 0; i < locs.length; i++) {
      marker_locations.push(new google.maps.LatLng(locs[i][0], locs[i][1]));
      latlngbounds.extend(marker_locations[i]);

      // Map is made available via ip_geoloc_gmap_multi_loc.js
      // in the ip_geoloc module.
      map.fitBounds(latlngbounds);
    }
  } else {
    // Set zoom to something, so its not so
    // zoomed in.
    map.setZoom(12);
  }
  var mcOptions = {gridSize: 50, maxZoom: 15};
  // TODO: Need an array of markers for clustering...
  var mc = new MarkerClusterer(map,[],mcOptions);
};
