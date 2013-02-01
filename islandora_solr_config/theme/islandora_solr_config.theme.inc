<?php

/**
 * @file
 * Theme functions for Islandora Solr Config
 */

/**
 * hook_preprocess function for islandora_solr_grid
 *
 * @global type $base_url
 * @global type $base_path
 * @param type $variables
 *   An associative array containing:
 *   - results: array of returned Solr documents
 *   - elements: (optional) additional elements to include in the template file
 *
 * @see islandora_solr_config_theme()
 * @see IslandoraSolrResultsGrid::printResults()
 */
function islandora_solr_config_preprocess_islandora_solr_grid(&$variables) {

  // Default thumbnail URL.
  $variables['elements']['solr_default_img'] = url(drupal_get_path('module', 'islandora_solr') . '/images/defaultimg.png');

  // Make all variables in 'elements' available as variables in the template file.
  foreach ($variables['elements'] as $key => $value) {
    $variables[$key] = $value;
  }
}
