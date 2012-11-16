<?php

/**
 * @file
 * This file contains DB query functions
 */

/**
 * Function to get result fields
 */
function islandora_solr_get_result_fields($filter = TRUE) {
  $records = islandora_solr_get_fields('result_fields', $filter);
  return $records;
}

/**
 * Function to get sort fields
 */ 
function islandora_solr_get_sort_fields($filter = TRUE) {
  $records = islandora_solr_get_fields('sort_fields', $filter);
  return $records;  
}

/**
 * Function to get facet fields
 */
function islandora_solr_get_facet_fields($filter = TRUE) {
  $records = islandora_solr_get_fields('facet_fields', $filter);
  return $records;
}
 
/**
 * Function to get search fields
 */
function islandora_solr_get_search_fields($filter = TRUE) {
  $records = islandora_solr_get_fields('search_fields', $filter);
  return $records;
}

/**
 * Generic function which performs a select query to get fields for any field type.
 *
 * @params string $field_type
 *   The type of field to query for. If not passed, the query returns all fields.
 * @params bool $filter
 *   If set to TRUE, it will filter the array agains user roles. If set to FALSE, it will return all results.
 * @return array
 *   The query results as an array.
 */
function islandora_solr_get_fields($field_type = NULL, $filter = TRUE) {
  // fields query
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
  // if filter is set to FALSE, then return all values (no user-role checking)
  if ($filter == FALSE) {
    return $records;
  }
  // else, we filter the fields permissions against the user role(s).
  $records = _islandora_solr_filter_fields_permissions($records);
  return $records;
}

/**
 * Checks an array of fields DB records against user roles
 *
 * @param array $records
 *   An array of fields.
 * @return array
 *   The array of fields, filtered by user roles.
 */
function _islandora_solr_filter_fields_permissions($records = array()) {
  global $user;
  // no checks for user 1
  if ($user->uid == 1) {
    return $records;
  }
  $filtered_records = array();
  foreach($records as $key => $record) {
    if (isset($record['solr_field_settings']) AND !empty($record['solr_field_settings'])) { // @TODO: problem here. If a user never has set any settings in the dialog, grant or deny access? This solution denies access.
      // get field permissions
      $field_roles = $record['solr_field_settings']['permissions'];
      // check field permissions agains user roles
      $check = array_intersect($field_roles, array_values($user->roles));
      if (!empty($check)) {
        $filtered_records[] = $record;
      }
    }
  }
  return $filtered_records;
}