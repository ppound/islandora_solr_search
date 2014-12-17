<?php
/**
 * @file
 * Islandora solr search return url for use on object pages
 *
 * Variables available:
 * - $islandora_solr_search_return_link: Link to return solr search.
 *
 * @see template_preprocess_islandora_solr_search_return_link()
 */
?>


  <div id="islandora-solr-search-return-link"><?php print $return_link; ?></div>
  <div id="islandora-solr-search-prev-link"><?php print $prev_link; ?></div>
  <div id="islandora-solr-search-next-link"><?php print $next_link; ?></div>

