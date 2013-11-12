<?php

/**
 * @file
 * Contains all administration related functionality for islandora_solr_geo.
 */

/**
 * The administration form for the islandora_solr_geo module.
 *
 * @param array $form
 *   An array representing the administration Drupal form.
 * @param array $form_state
 *   An array containing the Drupal form state.
 *
 * @return array
 *   The rendered Drupal form.
 */
function islandora_solr_geo_admin_form($form, $form_state) {
  $form = array();
  $form['islandora_solr_geo_coordinates_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Solr field'),
    '#description' => t('The Solr field to pull the coordinate data out of.'),
    '#autocomplete_path' => 'islandora_solr/autocomplete_luke',
    '#default_value' => variable_get('islandora_solr_geo_coordinates_field', 'mods_subject_cartographics_coordinates_ms'),
  );
  return system_settings_form($form);
}
