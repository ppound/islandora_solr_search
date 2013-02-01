<?php
/**
 * @file
 *   Islandora solr primary results template file for
 *
 * Variables available:
 * - $variables: all array elements of $variables can be used as a variable.
 *   e.g. $base_url equals $variables['base_url']
 * - $base_url: The base url of the current website. eg:
 *   http://example.com/drupal .
 * - $user: The user object.
 * - $solr_default_img: default solr image. Used when no thumbnail is available.
 *
 * - $results: Primary profile results array
 *
 * @see template_preprocess_islandora_solr_grid()
 */

?>

<?php if (empty($results)): ?>
  <p class="no-results"><?php print t('Sorry, but your search returned no results.'); ?></p>
<?php else: ?>
  <div class="islandora-solr-search-results">
    <div class="islandora-solr-grid clearfix">
    <?php foreach($results as $result): ?>
      <dl class="solr-grid-field">
        <dt class="solr-grid-thumb">
          <?php $image = '<img src="' . url($result['thumbnail_url'], array('query' => $result['thumbnail_url_params'])) . '" />'; ?>
          <?php print l($image, $result['object_url'], array('html' => TRUE, 'query' => $result['object_url_params']));?>
        </dt>
        <dd class="solr-grid-caption">
          <?php $title = isset($result['solr_doc']['dc.title']['value']) ? $result['solr_doc']['dc.title']['value'] : ''; ?>
          <?php print l($title, $result['object_url'], array('query' => $result['object_url_params']));?>
        </dd>
      </dl>
    <?php endforeach; ?>
    </div>
  </div>
<?php endif; ?>
