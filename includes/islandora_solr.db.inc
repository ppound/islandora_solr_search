<?php

/**
 * @file
 * This file contains DB query functions
 */

/**
 * Function to get result fields
 */
function islandora_solr_get_result_fields() {
  $records = islandora_solr_get_fields('result_fields');
  return $records;
}

/**
 * Function to get sort fields
 */ 
function islandora_solr_get_sort_fields() {
  $records = islandora_solr_get_fields('sort_fields');
  return $records;  
}

/**
 * Function to get facet fields
 */
function islandora_solr_get_facet_fields() {
  $records = islandora_solr_get_fields('facet_fields');
  return $records;
}
 
/**
 * Function to get search fields
 */
function islandora_solr_get_search_fields() {
  $records = islandora_solr_get_fields('search_fields');
  return $records;
}

/**
 * Generic function which performs a select query to get fields for any field type.
 *
 * @params string $field_type
 *   The type of field to query for. If not passed, the query returns all fields.
 * @return array
 *   The query results as an array.
 */
function islandora_solr_get_fields($field_type = NULL) {
  $query = db_select('islandora_solr_fields', 'isf');
  $query->fields('isf'); // all fields
  if ($field_type !== NULL) {
    $query->condition('isf.field_type', $field_type, '=');
  }
  $query->orderBy('isf.weight', 'ASC');
  $result = $query->execute();
  $records = $result->fetchAll(PDO::FETCH_ASSOC);
  // unserialize solr field settings blob
  foreach ($records as $key => $value) {
    $records[$key]['solr_field_settings'] = unserialize($records[$key]['solr_field_settings']);
  }
  return $records;
}