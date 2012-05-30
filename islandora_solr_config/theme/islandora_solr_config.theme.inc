<?php

/**
 * @file
 * Theme functions for Islandora Solr Config
 */

/**
 * hook_preprocess function for islandora_solr_grid
 */
function islandora_solr_config_preprocess_islandora_solr_grid(&$variables) {
  // base url
  global $base_url;
  // base path
  global $base_path;

  // set base url
  $variables['base_url'] = $base_url;

  // set base path
  $variables['base_path'] = $base_path;
  
  // get path
  $path = $base_path . drupal_get_path('module', 'islandora_solr');
  
  // placeholder image
  $solr_default_img = $path . '/images/defaultimg.png';
  $variables['elements']['solr_default_img'] = $solr_default_img;

  // make all variables in 'elements' available as variables in the template file
  foreach ($variables['elements'] as $key => $value) {
    $variables[$key] = $value;
  }
  
}