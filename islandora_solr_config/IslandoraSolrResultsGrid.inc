<?php

/**
 * @file
 * Class override for grid display
 */

/**
 * Extention of IslandoraSolrResults to create an alternative display type.
 */
class IslandoraSolrResultsGrid extends IslandoraSolrResults {

  /**
   * Renders the Solr results as a responsive grid view. Markup and styling is based
   * on the Islandora collection grid view. Some styling is inherited from it.
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

    // set variables
    $results_array = array();
    $elements = array();

    // total
    $elements['solr_total'] = $results->response->numFound;

    // start
    $elements['solr_start'] = $results->response->start;

    // get prepared search results
    $docs = $this->prep_results($results);

    // loop over results array
    foreach ($docs as $doc) {
      $rows = array();
      $row = 0; // keeping $row in there for now until the snippet issue is cleared out
      $snippet = NULL; // @TODO: how is this suposed to work if the $snippet always is set to NULL ?

      // loop over individual results to process fields
      foreach ($doc as $field => $value) {

        // check for field label substitutions
        $translated_field_name = isset($this->allSubsArray[$field]) ? $this->allSubsArray[$field] : $field;

        // add label
        $rows[$field]['label'] = $translated_field_name;

        // add class: render solr field name to string that can be used as class name
        $rows[$field]['class'] = drupal_strtolower( preg_replace('/[^A-Za-z0-9]/', '-', $field));

        // split value if the result value is an array
        if (is_array($value)) {
          $value = implode(", ", $value);
        }
        // add value
        $rows[$field]['value'] = $value;

        $row++; // keeping $row in there for now until the snippet issue is cleared out
      }
      // @TODO: how is this suposed to work if the $snippet always is set to NULL ?
      if ($snippet) {
        $rows[$row][] = array(
          'data' => 'Full Text',
          'header' => TRUE,
        );
        $rows[$row][] = $snippet[0];
      }
      // append array to results array
      $results_array[] = $rows;
    }

    // return themed search results
    return theme('islandora_solr_grid', array('results' => $results_array, 'elements' => $elements));
  }



}

