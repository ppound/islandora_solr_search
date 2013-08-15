<?php

/**
 * @file
 * Features hook implementations.
 */

/**
 * Implements hook_features_export_options().
 */
function islandora_solr_fields_features_export_options() {
  $field_types = array(
    'result_fields' => t('Display fields'),
    'sort_fields' => t('Sort fields'),
    'facet_fields' => t('Facet fields'),
    'search_fields' => t('Advanced search fields'),
  );

  return $field_types;
}

/**
 * Implements hook_features_export().
 */
function islandora_solr_fields_features_export($data, &$export, $module_name = '') {
  $export['dependencies']['islandora_solr'] = 'islandora_solr';

  return $export;
}

/**
 * Implements hook_features_export_render().
 */
function islandora_solr_fields_features_export_render($module, $data, $export) {
  $code = array();
  foreach ($data as $field_type) {
    $code[$field_type] = islandora_solr_get_fields($field_type, FALSE, FALSE);
  }

  $code = "  return " . features_var_export($code, '  ') . ";";
  return array('islandora_solr_fields_features_default_fields' => $code);
}

function islandora_solr_fields_features_revert($module_name) {
  islandora_solr_fields_features_rebuild($module_name);
}

/**
 * Helper function to serialize a particular array entry.
 *
 * @param array $field
 *   An array describing a field in the "islandora_solr_fields" table, with the
 *   "solr_field_settings" entry being an array. If it's a string, we let it
 *   pass through untouched. 
 *
 * @return array
 *   The input with "solr_field_settings" serialize with serialize().
 */
function islandora_solr_serialize_solr_field_settings($field) {
  if (!is_string($field['solr_field_settings'])) {
    $field['solr_field_settings'] = serialize($field['solr_field_settings']);
  }

  return $field;
}

/**
 * Implements hook_features_rebuild().
 */
function islandora_solr_fields_features_rebuild($module_name) {
  $values = module_invoke($module_name, 'islandora_solr_fields_features_default_fields');
  if (!empty($values)) {
    foreach ($values as $field_type => $fields) {
      db_delete('islandora_solr_fields')
        ->condition('field_type', $field_type)
        ->execute();
      if (count($fields) > 0) {
        $i = db_insert('islandora_solr_fields')
          ->fields(array(
            'solr_field',
            'field_type',
            'weight',
            'solr_field_settings',
        ));

        // Serialize the "solr_field_settings" bit.
        $fields = array_map('islandora_solr_serialize_solr_field_settings', $fields);

        // Write to our database table
        foreach ($fields as $field) {
          $i->values($field); 
        }
        $i->execute();
      }
    }
  }
}
