<?php

/**
 * @file
 * Class override for display on a map.
 */

/**
 * Extension of IslandoraSolrResults to create an alternative display type.
 */
class IslandoraSolrResultsGeo extends IslandoraSolrResults {

  /**
   * Renders the Solr results as a responsive map view.
   *
   * Markup and styling is based on the Islandora collection grid view. Some
   * styling is inherited from it.
   *
   * @see IslandoraSolrResults::displayResults()
   *
   * @param array $solr_results
   *   The processed Solr results from
   *   IslandoraSolrQueryProcessor::islandoraSolrResult.
   *
   * @return string
   *   Rendered Solr results.
   */
  public function printResults($solr_results) {
    return theme('islandora_solr_geo_display_results', $solr_results);
  }
}
