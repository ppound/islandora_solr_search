<?php

/*
 * @file
 * Contains methods to search solr and display results. Depends on
 * Apache_Solr_Php client.
 */

/**
 * Extention of IslandoraSolrResults to create an alternative display type.
 */
class IslandoraSolrResultsTable extends IslandoraSolrResults {

  function IslandoraSolrResultsTable() {
    parent::__construct();
  }

  /**
   * Renders the Solr results as a table.
   *
   * @param type $results
   *   The raw Solr results from IslandoraSolrQueryProcessor::solrResult
   * @return
   *   Rendered Solr results
   *
   * @see IslandoraSolrResults::displayResults()
   */
  function printResults($results) {

    // add islandora solr theme css
    drupal_add_css(drupal_get_path('module', 'islandora_solr_config') . '/css/islandora_solr_config.theme.css');

    // set base_url
    global $base_url;
    // set variable
    $output = '';

    $recordStart = (int)$results->response->start;
    if (empty($results)) {
      return t('no results');
    }
    $count = 0;
    $fields = array();
    $values = array();
    $header = array();
    $limitResults = variable_get('islandora_solr_limit_result_fields', 0);

    $object_link_placeholder = "~object_href~";

    foreach ($results->response->docs as $doc) {
      unset($pid);
      foreach ($doc as $field => $value) {

        if ($limitResults && empty($this->resultFieldArray[$field])) {
          continue;
        }

        // catches the object PID.
        if ($field == 'PID') {
          $pid = $value;
          // we need this in the resultset, but would prefer to hide it from users.
        }

        // Sometimes we get a single value, sometimes an array.
        // For simplicity, I push the singles into an array and later on process each
        // field as an array.
        if (!is_array($value)) {
          $value = array($value);
        }

        // We're going to put the object link onto the dublin-core title.
        if ($field == 'dc.title' || $field == 'dc:title') {
          foreach ($value as $key => $val) {
            if (isset($doc->PID)) {
              $value[$key] = l($val, 'islandora/object/' . $doc->PID . '/-/' . $val);
            }
          }
        }
        $value = implode(", ", $value);

        //this would change 'animal_name' to the more readable 'Animal Name'
        //$field = ucwords( str_replace( '_', ' ', $field ) );

        // Add this field to the master list of fields if it's not there already:
        if (!in_array($field, $fields)) {
          $fields[] = $field;
        }

        $values[$count][$field] = $value;
      }
      $count++;
    }

    foreach ($fields as $field) {
      // Get human-friendly field names;
      $translated_field_name = isset($this->allSubsArray[$field]) ? $this->allSubsArray[$field] : $field;
      $field_array = array(
        'data' => $translated_field_name,
        'field' => NULL,
        'sort' => NULL,
        'title' => $field,
      );
      $header[] = $field_array;
    }
    array_unshift($header, '#');

    // set rows variable
    $rows = array();
    // loop over each row
    foreach ($values as $num => $val_array) {
      $row = array();
      unset($rowclass);
      $result_num = $recordStart + $num + 1;
      $row['data'][] = array(
        'data' => (isset($val_array['PID']) ? l($result_num, 'islandora/object/' . $val_array['PID']) : $result_num),
        'header' => TRUE,
//        'class' => 'col-first'

      );
      $field_count = count($fields);
      // loop over each cell within a row
      foreach ($fields as $fieldnum => $field) {
        $data = isset($val_array[$field]) ? $val_array[$field] : ' ' ;
        $cell_data = array('data' => $data);

        $row['data'][] = $cell_data;

      }
      $rows[] = $row;
    }
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('islandora-solr-results-table'))));

    if (variable_get('islandora_solr_debug_mode', 0)) { // debug dump
      $results_r .= "<pre>Results: " . print_r($results, TRUE) . "</pre>";
      $fieldset_r = array(
        '#title' => t("Raw Results"),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#value' => $results_r,
        );
      $output .= theme('fieldset', $fieldset_r);
    }
    return $output;
  }

}

