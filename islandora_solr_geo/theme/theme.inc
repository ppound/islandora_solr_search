<?php
/**
 * @file
 * Theme functions for the islandora_solr_geo module.
 */

/**
 * Implements hook_preprocess().
 */
function islandora_solr_geo_preprocess_islandora_solr_geo_display_results(&$variables) {
  $mod_path = drupal_get_path('module', 'islandora_solr_config');
  $geo_mod_path = drupal_get_path('module', 'islandora_solr_geo');

  drupal_add_css("$mod_path/css/islandora_solr_config.theme.css");
  drupal_add_js($geo_mod_path . "/js/islandora_solr_geo_gmap_utils.js");
  drupal_add_js($geo_mod_path . "/js/markerclusterer.js");

  $elements = array();
  $locations = array();
  $map_variables = array();
  // For use with extents in Google Maps api, javascript side.
  $lati_long = array();
  $elements['solr_total'] = $variables['response']['numFound'];
  $elements['solr_start'] = $variables['response']['start'];
  foreach ($variables['response']['objects'] as $result) {
    $coordinate_field = variable_get('islandora_solr_geo_coordinates_field', 'mods_subject_cartographics_coordinates_ms');
    // This object has lat lon, show on map.
    if (isset($result['solr_doc'][$coordinate_field]) && isset($result['solr_doc'][$coordinate_field][0])) {
      // Split this on ','.
      $coords = explode(",", $result['solr_doc'][$coordinate_field][0]);
      array_push($lati_long, array($coords[0], $coords[1]));
      $balloon_variables = array();
      $balloon_variables['solr_result'] = $result;
      $balloon_markup = theme('islandora_solr_geo_balloon', $balloon_variables);
      $location = new IslandoraMapObjectLocation(NULL, $coords[0], $coords[1], $balloon_markup);
      array_push($locations, $location);
    }
  }
  drupal_add_js($geo_mod_path . "/js/islandora_solr_geo_gmap_utils.js");
  drupal_add_js(array(
    'islandora_solr_geo' => array(
      'locations' => $lati_long,
    ),
  ), 'setting');
  $map_variables['locations'] = $locations;
  $map_variables['map_options'] = '{"mapTypeId":"roadmap", "zoom":50}';
  $map_variables['map_style'] = "height: 550px; width:100%;";

  $variables['results'] = theme(
    'islandora_mapping_map_multi_item',
    $map_variables
  );
}

/**
 * Implements hook_preprocess().
 */
function islandora_solr_geo_preprocess_islandora_solr_geo_balloon(&$variables) {
  $result = $variables['solr_result'];
  $balloon_markup = array();
  $balloon_markup['object_label'] = array(
    '#markup' => $result['object_label'],
  );
  $balloon_markup['object_thumbnail'] = array(
    '#theme' => 'image',
    '#path' => $result['thumbnail_url'],
    '#alt' => $result['PID'],
  );
  $balloon_markup['object_link'] = array(
    '#markup' => l($result['PID'], $result['object_url']),
    '#suffix' => '<br/>',
  );
  $balloon_markup['object_description'] = array(
    '#markup' => 'Description: <br/>' . truncate_utf8($result['solr_doc']['dc.description'][0], 50, FALSE, TRUE),
  );
  $variables['balloon'] = $balloon_markup;
  $variables['balloon_markup'] = drupal_render($balloon_markup);
}
