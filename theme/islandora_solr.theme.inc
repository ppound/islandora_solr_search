<?php

/**
 * @file
 * Theme functions for the Islandora Solr Search module.
 */


/**
 * hook_preprocess function for islandora_solr_wrapper
 *
 * @global type $base_url
 * @param type $variables
 */
function islandora_solr_preprocess_islandora_solr_wrapper(&$variables) {
  // base url
  global $base_url;
  $variables['base_url'] = $base_url;

  // elements
  $elements = $variables['elements'];

  // make all variables in 'elements' available as variables in the template file
  foreach ($variables['elements'] as $key => $value) {
    $variables[$key] = $value;
  }
  // results count
  $total = $elements['solr_total'];
  $end = $elements['solr_end'];
  $start = $elements['solr_start'] + ($total > 0 ? 1 : 0);

  // format numbers
  $total = number_format($total , 0, '.', ',');
  $end = number_format($end , 0, '.', ',');
  $start = number_format($start , 0, '.', ',');
  // construct string
  $islandora_solr_result_count = t('(@start - @end of @total)', array('@start' => $start, '@end' => $end, '@total' => $total));
  // add to variables
  $variables['islandora_solr_result_count'] = $islandora_solr_result_count;

}



/**
 * hook_preprocess function for islandora_solr_default
 *
 * @global type $base_url
 * @global type $base_path
 * @param type $variables
 */
function islandora_solr_preprocess_islandora_solr(&$variables) {
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
