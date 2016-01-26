<?php

/**
 * @file
 * Form for collection sort settings.
 */

/**
 * Form to set collection sort string.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $object
 *   The collection object.
 *
 * @return array
 *   The collection sort management form.
 */
function islandora_solr_manage_collection_sort_form(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_solr', 'includes/db');
  $current_default = islandora_solr_get_collection_sort_string($object->id);
  $form_state['collection'] = $object->id;
  $collection_sort = variable_get('islandora_solr_collection_sort', 'Not set');
  $base_sort = variable_get('islandora_solr_base_sort', 'Not set');
  return array(
    '#action' => request_uri() . '#manage-collection-solr-sort',
    'collection_sort_string' => array(
      '#type' => 'textfield',
      '#title' => t('Solr Collection Sort String'),
      '#description' => t('A string representing a sort parameter to add if using the Solr collection query backend. Can be set to empty to fall back to the global sort setting for collections, or the global base sort setting if there is none. Note that adding multivalued or otherwise unsortable fields here will break the collection view.</br>
      Global Collection Sort: %collection_sort</br>
      Global Base Sort: %base_sort</br>', array(
        '%collection_sort' => empty($collection_sort) ? "Empty" : $collection_sort,
        '%base_sort' => empty($base_sort) ? "Empty" : $base_sort,
      )),
      '#default_value' => (string) $current_default,
      // These strings can get big haha ...
      '#size' => 100,
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Apply'),
    ),
  );
}

/**
 * Submit handler for the collection sort management form.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_solr_manage_collection_sort_form_submit(array $form, array &$form_state) {
  islandora_solr_set_collection_sort_string($form_state['collection'], $form_state['values']['collection_sort_string']);
}
