<?php

/**
 * @file
 * This file contains DB query functions
 */

function islandora_solr_get_result_fields() {
  $records = islandora_solr_get_fields('result_fields');
  return $records;
}
 
function islandora_solr_get_sort_fields() {
  $records = islandora_solr_get_fields('sort_fields');
  return $records;  
}
 
function islandora_solr_get_facet_fields() {
  $records = islandora_solr_get_fields('facet_fields');
  return $records;
}
 
function islandora_solr_get_search_fields() {
  $records = islandora_solr_get_fields('search_fields');
  return $records;
}

function islandora_solr_get_fields($field_type) {
  $query = db_select('islandora_solr_fields', 'isf');
  $query->fields('isf'); // all fields
  $query->condition('isf.field_type', $field_type, '=');
  $query->orderBy('isf.weight', 'ASC');
  $result = $query->execute();
  $records = $result->fetchAll(PDO::FETCH_ASSOC);
  
  foreach ($records as $key => $value) {
    $records[$key]['solr_field_settings'] = unserialize($records[$key]['solr_field_settings']);
  }
  
  return $records;
}