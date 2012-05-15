<?php

/**
 * @file
 * Contains methods to search solr and display results.  depends on Apache_Solr_Php client.
 */

/**
 * Islandora Solr Results
 */
class IslandoraSolrResults {

  static $facetSeparator = '~'; //used to separate facets in url
  static $slashReplacement = '~slsh~'; // a pattern used to replace / in url's the slash breaks drupal clean url's
  public static $SEARCH_CLASS_ADVANCED_SEARCH_NUMBER_FIELDS = 5;
  public $facetFieldArray = array();
  public $searchFieldArray = array();
  public $resultFieldArray = array();
  public $allSubsArray = array();

  /**
   * Constructor
   */
  function IslandoraSolrResults() {
    if (module_exists('apachesolr')) {
      module_load_include('php', 'apachesolr', 'SolrPhpClient/Apache/Solr/Service');
    }
    else {
      module_load_include('php', 'islandora_solr_search', 'SolrPhpClient/Apache/Solr/Service');
    }
    module_load_include('inc', 'islandora_solr_search', 'includes/common');
    $this->prepFieldSubstitutions();
  }

  /**
   * Output the main body of the search results
   *
   * @param type $solrQueryProcessor
   * @param type $title
   * @param type $output
   * @return string
   */
  function displayResults($solrQueryProcessor, $title = NULL, $output = '') {
    
    // set variables to collect returned data.
    $results = NULL;
    $secondary_profiles = NULL;
    $elements = array();

    // set title
    if (!is_null($title)) {
      drupal_set_title($title);
    }
    
    // set breadcrumbs
    $this->setBreadcrumbs($solrQueryProcessor);
    
    // raw solr results
    $apacheSolrResult = $solrQueryProcessor->solrResult;
    
    // solr results count
    // total solr results
    $total = (int) $apacheSolrResult->response->numFound;
    $elements['solr_total'] = $total;

    // solr start
    // to display: $solrQueryProcessor->solrStart + ($total > 0 ? 1 : 0)
    $elements['solr_start'] = $solrQueryProcessor->solrStart;

    // solr results end
    $end = min(($solrQueryProcessor->solrLimit + $solrQueryProcessor->solrStart), $total);
    $elements['solr_end'] = $end;

    // pager
    islandora_solr_search_pager_init($total, $solrQueryProcessor->solrLimit);
    $elements['solr_pager'] = theme('pager', NULL, $solrQueryProcessor->solrLimit, 0, NULL, 5);

    // debug (will be removed)
    if (variable_get('islandora_solr_search_debug_mode', FALSE)) {
      $elements['solr_debug'] = $this->printDebugOutput($solrResult);
    }

    // rendered secondary display profiles
    $secondary_profiles = $this->addSecondaries();
    
    // rendered results
    $results = $this->printResults($apacheSolrResult);
        
    // return themed layout
    return theme('islandora_solr_wrapper', $results, $secondary_profiles, $elements);
  }

  function addSecondaries() {
    $output = '';
    // Get secondary display profiles
    $secondary_display_profiles = module_invoke_all('islandora_solr_secondary_display');
    // How about this for an expected return?
    // return array(
    // 'machine-name' = array(
    //   'name' => 'Human Readable Name',
    //   'module' => 'module_name',
    //   'file' => 'FileName.inc',
    //   'class' => 'ClassName',
    //   'function' => 'function_name',
    //   'description' => 'A description of the display profile',
    //   'logo' => 'Some text, some <html>, an <img>, etc used to link to this output,
    // );
    // check if
    // if the islandora_solr_search admin form has never been saved, then
    // the variable 'islandora_solr_secondary_display' won't be set yet.
    $secondary_array = variable_get('islandora_solr_secondary_display', array());

    // count number of elements in array
    $array_count = count($secondary_array);
    // count number of disabled displays
    $i = 0;
    foreach ($secondary_array as $key => $value) {
      if ($value === 0) {
        $i++;
      }
    }

    // if number of elements is the same as the number of disabled displays,
    // this means that there are no displays enabled.
    if ($array_count != $i) {

      foreach ($secondary_array as $name => $status) {
        if ($status === $name) {

          $description = $secondary_display_profiles[$name]['description'];
          $logo = $secondary_display_profiles[$name]['logo'];

          $attributes = array('title' => $description);
          $query = array();
          $query[] = drupal_query_string_encode(array('solr_profile' => $name));
          $output .= l($logo, $_GET['q'], array('attributes' => $attributes, 'html' => TRUE, 'query' => count($query) ? implode('&', $query) : NULL));
        }
      }

    }
    return $output;
  }

  /**
   * Function: printResults
   *
   * Description: translates a solr query result into a basic in-browser search result.
   *
   * @param Apache_Solr_Object $results
   */
  function printResults($results) {
    // set variables
    $results_array = array();
    $elements = array();
    
    // total
    $elements['solr_total'] = $results->response->numFound;

    // start
    $elements['solr_start'] = $results->response->start;
    
    // get prepared search results
    $docs = $this->prep_results($results);

    // optionally limit results to values given
    $limitResults = variable_get('islandora_solr_search_limit_result_fields', 0);

    // loop over results array
    foreach ($docs as $doc) {
      $rows = array();
      $row = 0; // keeping $row in there for now until the snippet issue is cleared out
      $snippet = NULL; // @TODO: how is this suposed to work if the $snippet always is set to NULL ?

      // loop over individual results to process fields
      foreach ($doc as $field => $value) {

        // check if this field should be included
        if ($limitResults && empty($this->resultFieldArray[$field])) {
          continue;
        }

        // check for field label substitutions
        $translated_field_name = isset($this->allSubsArray[$field]) ? $this->allSubsArray[$field] : $field;

        // add label
        $rows[$field]['label'] = $translated_field_name;

        // add class: render solr field name to string that can be used as class name
        $rows[$field]['class'] = strtolower( preg_replace('/[^A-Za-z0-9]/', '-', $field));

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
    return theme('islandora_solr_search', $results_array, $elements);
  }
  
  
  
  

  function printDebugOutput($results) {
    // debug dump
    $results_r .= "<pre>Results: " . print_r($results, TRUE) . "</pre>";
    $fieldset_r = array(
      '#title' => t("Raw Results"),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#value' => $results_r,
    );
    return theme('fieldset', $fieldset_r);
  }

  const FIELD_ALL = 0, FIELD_SEARCH = 1, FIELD_FACET = 2, FIELD_RESULT = 3;

  function _make_pattern($string) {
    return "/^" . preg_quote($string, '/') . "/";
  }

  function getSubstitutedFields($string_in, $type = FIELD_SEARCH) {
    $replacements = NULL;
    switch ($type) {
      case FIELD_SEARCH:
        $replacements = $this->searchFieldArray;
        break;
      case FIELD_FACET:
        $replacements = $this->facetFieldArray;
        break;
      case FIELD_RESULT:
        $replacements = $this->resultFieldArray;
        break;
      case FIELD_ALL:
      default:
        $replacements = $this->allSubsArray;
    }
    return preg_replace(array_map(array($this, '_make_pattern'), array_keys($replacements)), array_values($replacements), $string_in);
  }

  /**
   * Function: currentQuery
   *
   * Description: Displays elements of the current solr query (with enabled filters).
   *   This functionality was previously rolled into the facet block, but it makes
   *   sense to separate the two functions, particularly as the current query is now
   *   imprinted in the system's breadcrumb.
   *
   * @param IslandoraSolrQueryProcessor $solrQueryProcessor
   */
  function currentQuery($solrQueryProcessor) {
    // set variables
    $output = '';
    // get filtered fields passed through url
    $fq = $solrQueryProcessor->internalSolrParams['f'];
    $islandora_facets = isset($solrQueryProcessor->solrResult->facet_counts->facet_fields) ? $solrQueryProcessor->solrResult->facet_counts->facet_fields : NULL;
    
    if (drupal_strlen(trim($solrQueryProcessor->solrQuery))) {

      // Variables to build a drupal-ish unordered list
      $query_list_title = t('Query');
      $query_list_items = array();
      $query_list_type = "ul";
      $query_list_attributes = array('class' => 'islandora_solr_search_query_list query_list');

      // OK, so it's a list of one.
      // In the future, we could reasonably break the query on boolean operators
      // and allow one part to be removed at a time.
      $query = " ";
      $repr = '-'; //Again, don't explicitly specify an image...
      $link = 'islandora/solr/search/' . $query;
      $query_list_items[] .= l($repr, $link, array('attributes' => array('class' => 'islandora_solr_add_remove_link'), 'query' => $solrQueryProcessor->internalSolrParams))
              . ' '
              . $this->getSubstitutedFields(stripslashes($solrQueryProcessor->solrQuery), FIELD_SEARCH);

      $output .= '<div class="islandora_solr_search_query_wrap">';
      $output .= theme_item_list($query_list_items, $query_list_title, $query_list_type, $query_list_attributes);
      $output .= '</div>'; // 'islandora_solr_search_query_wrap'
    }

    //////////////////////////////////////////////////////////////////////////////
    // Here we will build links to remove currently enforced facet filters ...  //
    //////////////////////////////////////////////////////////////////////////////
    $filters = empty($islandora_fq) ? NULL : explode(IslandoraSolrResults::$facetSeparator, $islandora_fq);

    if (!empty($filters) && $islandora_fq != '-') {
//      $filter_output = NULL;
      // Variables to build a drupal-ish unordered list, which will be rendered
      // a little bit farther down
      $filter_list_title = t("Enabled Filters");
      $filter_list_items = array();
      $filter_list_type = "ul";
      $filter_list_attributes = array('class' => 'islandora_solr_search_filter_list filter_list');

      foreach ($filters as $key => $filter) {
        $edit_filters = $filters;
        unset($edit_filters[$key]);
        $filter_disable = implode(IslandoraSolrResults::$facetSeparator, $edit_filters);
        if (empty($filter_disable)) {
          $filter_disable = '-';
        }
        $filter_name_and_value = explode(":", $filter, 2);
        $filter_name = $filter_name_and_value[0];
        $filter_value = $filter_name_and_value[1];
        if (strncmp($filter_name, '-', 1))
          $exclude = FALSE;
        else {
          $exclude = TRUE;
          $filter_name = drupal_substr($filter_name, 1);
        }

        $query = replaceSlashes($solrQueryProcessor->solrQuery);
        $fq = replaceSlashes($filter_disable);
        $defType = $solrQueryProcessor->solrDefType;
        $repr = '-'; //I know that Kris has complained about the remove image before...  This should allow it to be changed.
        $link = "islandora/solr/search/$query/$fq/$defType/$display_param";
        $filter_list_items[] = l($repr, $link,
                array(
                  'attributes' => array(
                    'class' => 'islandora_solr_add_remove_link'))
            ) . ' ' . $this->facetFieldArray[$filter_name] . ($exclude ? ' != ' : ' = ') . $filter_value;
      }
    }

    if (!empty($filter_list_items)) {
//      $filter_output = str_replace('%2B','%252B',$filter_output);
      $output .= '<div class="islandora_solr_search_filter_wrap">';
      $output .= theme_item_list($filter_list_items, $filter_list_title, $filter_list_type, $filter_list_attributes);
      $output .= '</div>'; // 'islandora_solr_search_filter_wrap'
    }

    return $output;
  }

  /**
   * Function: setBreadcrumbs
   * 
   * Description: sets the drupal breadcrumbs based on the current query and filters. 
   */
  function setBreadcrumbs($solrQueryProcessor) {
    dsm($solrQueryProcessor);
    
    // set url
    $queryurl = SOLR_SEARCH_PATH . "/" . replaceSlashes($solrQueryProcessor->solrQuery);
    
    $breadcrumb_fq = $islandora_fq;
    $facet = NULL; // Nigelb, This is being used outside of the for loop where it is being created this is probably incorrect.
    if (strcmp($islandora_fq, "-")) {
      foreach (csv_explode(IslandoraSolrQueryProcessor::$facetSeparator, $islandora_fq, '"', TRUE) as $facet) {
        $field = drupal_substr($facet, 0, strpos($facet, ":"));
        $value = restoreSlashes(trim(drupal_substr($facet, strpos($facet, ":") + 1), '"'));
        $options = array('html' => TRUE);
        $options['attributes']['title'] = $facet;
        $crumblink = $queryurl . "/" . $breadcrumb_fq . (empty($this->solrDefType) ? '/-' : '/' . $solrQueryProcessor->solrDefType) . '/' . $display_param;
        $cut_fq = $solrQueryProcessor->delete_filter($islandora_fq, $facet);
        $cutlink = $queryurl . "/" . $cut_fq . (empty($solrQueryProcessor->solrDefType) ? '/-' : '/' . $solrQueryProcessor->solrDefType) . '/' . $display_param;
        if (!strncmp("-", $facet, 1)) {
          $options['attributes']['class'] = "strikethrough";
        }
        $breadcrumb[] = l($value, $crumblink, $options) .
            " <span class='islandora_solr_search_breadcrumb_super'>(" . l(t('x'), $cutlink, array('attributes' => array('title' => "Remove " . $facet))) . ")</span>";
        $breadcrumb_fq = $solrQueryProcessor->delete_filter($breadcrumb_fq, $facet);
      }
    }
    
    
    if (!in_array($solrQueryProcessor->solrQuery, $solrQueryProcessor->different_kinds_of_nothing)) {
      $cutlink = SOLR_SEARCH_PATH . "/ /" . $islandora_fq . '/-/' . $display_param;
      $queryval = $solrQueryProcessor->solrQuery;
      $tokens = explode(' ', $queryval);
      foreach ($tokens as $token) {
        preg_match("/^.*:/", $token, $matches);
        if (isset($matches[0])) {
          $queryval = str_replace($matches[0], "", $queryval);
        }
      }
      if ($queryval != '%252F') {
        $queryurl = "$queryurl/-/{$solrQueryProcessor->solrDefType}/$display_param";
        $breadcrumb[] = l($queryval, $queryurl, array('attributes' => array('title' => $solrQueryProcessor->solrQuery))) .
            " <span class='islandora_solr_search_breadcrumb_super'>(" . l(t('x'), $cutlink, array('attributes' => array('title' => "Remove " . $facet))) . ")</span>";
      }
    }
    
    
    $breadcrumb[] = l(t('Home'), NULL);
    if (!empty($breadcrumb))
      $breadcrumb = array_reverse($breadcrumb);
    drupal_set_breadcrumb($breadcrumb);
    
  }
  
  /**
   * Function: prepFieldSubstitutions
   *
   * Description: reads configuration values and preps a number of key=>value
   *   arrays for output substitution
   */
  function prepFieldSubstitutions() {
    $rawFacetVals = variable_get("islandora_solr_search_block_facets", 'dc.subject ~ Subject,dc.type ~ Type');
    $this->facetFieldArray = islandora_build_substitution_list($rawFacetVals);

    $rawSearchTerms = variable_get('islandora_solr_searchterms', 'dc.title ~ Title');
    $this->searchFieldArray = islandora_build_substitution_list($rawSearchTerms);

    $rawResultFields = variable_get('islandora_solr_search_result_fields', 'dc.subject ~ Subject,dc.type ~ Type');
    $this->resultFieldArray = islandora_build_substitution_list($rawResultFields);

    $this->allSubsArray = array_merge($this->facetFieldArray, $this->searchFieldArray, $this->resultFieldArray);
  }


  /**
   * Function: displayFacets
   *
   * Description: Displays basic facets based on an apache solr query response,
   *   as contained with the IslandoraSolrQueryProcessor.
   *
   * @global string $base_url
   * @param IslandoraSolrQueryProcessor $solrQueryProcessor
   * @return string
   */
  function displayFacets($solrQueryProcessor) {
    // set variables
    $facet_output = $output = '';
    
    // shown limit
    $facet_min_count = variable_get('islandora_solr_search_block_facet_min_count', '2');
    $initial_limit = variable_get('islandora_solr_search_block_facet_shown_limit', '0');
    $facet_limit = variable_get('islandora_solr_search_block_facet_limit', '20');
    
    // get facet values returned from solr
    $facets = isset($solrQueryProcessor->solrResult->facet_counts->facet_fields) ? $solrQueryProcessor->solrResult->facet_counts->facet_fields : array();

    // return if no facets are available
    if (empty($facets)) {
      return $output;
    }

    // set show more variable
    $show_more = FALSE;

    // loop over returned facet objects
    foreach ($facets as $facet_key => $facet) {
      $list_items = array();
      $list_type = "ul";
      $list_title = NULL;

      // convert to array
      $facet = get_object_vars($facet);
      
      // if the facet contains the minimum amount of values
      if (count($facet) >= $facet_min_count) {
        // set facet count
        $facet_count = 0;

        // render facet values
        foreach ($facet as $value => $number) {
          // add facet
          $this->_addFacets($facet_key, $value, $number, $solrQueryProcessor, $facet_count, $list_items);
        }

        // check for minumum returned values again, because some might have been disabled
        if (count($list_items) >= $facet_min_count) {
          
          // opening facet div // @TODO: this should go in theme function
          $facet_output .='<div class="islandora_solr_search_facet">';
          
          // set title
          $list_title = $this->facetFieldArray[$facet_key];

          // set attributes
          $list_attributes = array('class' => 'islandora_solr_search_facet_list facet_list');

          // check if facet value amount is larger than the initial limit
          $split_facet = $facet_count > $initial_limit AND
              $initial_limit < $facet_limit AND
              $initial_limit > $facet_min_count;

          if ($split_facet) {
            $show_more = TRUE; //There exists a split array...
            $list_items_hidden = array_slice($list_items, $initial_limit);
            $list_items = array_slice($list_items, 0, $initial_limit);
          }

          // theme the facet as a list
          $facet_output .= theme('item_list', $list_items, $list_title, $list_type, $list_attributes);

          // render the hidden facets
          if ($split_facet) {
            $facet_output .= '<a href="#" class="shown-toggle">' . t('Show more') . '</a>';
            $facet_output .= '<a href="#" class="shown-toggle hidden">' . t('Show less') . '</a>';
            $facet_output .= '<div class="toggle-wrapper hidden">';
            $facet_output .= theme('item_list', $list_items_hidden, NULL, $list_type, $list_attributes);
            $facet_output .= '</div>';
          }

          $facet_output .='</div>'; //div.islandora_solr_search_facet
        }
      }
    }

    if ($show_more) {
      // if there is at least one occurence of a split array, we add js to the page.
      drupal_add_js(drupal_get_path('module', 'islandora_solr_search') . '/js/islandora_solr_search_shown_limit.js');
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // as we add additional facets, we're repeatedly URL-encoding old facet             //
    // strings. when we double-encode quotation marks they're incomprehensible to solr. //
    // This is a quick workaround:                                                      //
    //////////////////////////////////////////////////////////////////////////////////////
    $facet_output = str_replace('%2B', '%252B', $facet_output);
    if (!empty($facet_output)) {
      $output .='<div class="islandora_solr_search_facet_wrap">';
      $output .= $facet_output;
      $output .= '</div>';
    }

    return $output;
  }

  
  /**
   * Add facet callback
   * @param type $key
   * @param type $name
   * @param type $number
   * @param type $solrQueryProcessor
   * @param int $facet_count
   * @param string $list 
   */
  function _addFacets(&$key, &$value, &$number, &$solrQueryProcessor, &$facet_count, &$list) {
    
    //FIXME:  This isn't quite right...  It can make sense to facet on 
    //  the same field multiple times, provided it is multi-valued...
    $fq = (array) $solrQueryProcessor->solrParams['fq'];
    
    // @TODO: Should be changed to format_string for Drupal 7
    $filter = $key . ':"' . replaceSlashes($value) . '"'; // PID:"foo"

    // check if item should show up
    $disable_link = (array_search($filter, $fq) !== FALSE) || $number == $solrQueryProcessor->solrResult->response->numFound;


    
    // if link isn't disabled
    if (!$disable_link) {

      // set variables
      $path = SOLR_SEARCH_PATH . '/' . $solrQueryProcessor->solrQuery; // $_GET['q'] didn't seem to work here
      // get parameters set in the url
      $params = $solrQueryProcessor->internalSolrParams;
      
      if (empty($params)) {
        $params = array();
      }
      
     
      $query_plus = $params;
      $query_minus = $params;
     
      // include string
      $query_plus['f'][] = $filter;
   
      // exclude string
      $query_minus['f'][] = '-' . $filter;
      //$query_minus['f'][] = '-' . $filter;
      
      // set basic attributes
      $attributes = array('rel' => 'nofollow');

      $attributes_value = $attributes_plus = $attributes_minus = $attributes;
      
      $attributes_value['href'] = url($path, array('query' => $query_plus));
      $attributes_plus['href'] = url($path, array('query' => $query_plus));
      $attributes_minus['href'] = url($path, array('query' => $query_minus));
      
      $attributes_plus['class'] = 'plus';
      $attributes_minus['class'] = 'minus';
      
      // create links
      // we're not using l() because of active classes: http://drupal.org/node/41595
      $list[] = '<a' . drupal_attributes($attributes_value) . '>' . $value . '</a>' . ' (' . $number . ')' .
          '<span class="plusminus">' .
          '<a' . drupal_attributes($attributes_plus) . '>+</a>'
          . ' ' .
          '<a' . drupal_attributes($attributes_minus) . '>-</a>'
          . '</span>';
      
      $facet_count++;
      

      
    }
  }
  
  
  
  function _add_field(&$form, &$i, &$unprocessed_contents, &$types) {
    $type = NULL;
    $temp = preg_split('/:/', $unprocessed_contents, 2);

    if (!isset($type)) {
      if (trim($temp[0]) === '_query_') {
        //We're using a subquery--let's assume dismax
        //get the 'qf'..., dump the rest into 'terms'
        $temp[1] = trim($temp[1]);
        $qf = strpos($temp[1], 'qf=');
        $end = strpos($temp[1], '}');
        $type = substr($temp[1], $qf + 3, $end - $qf - 3);
        $terms = trim(substr($temp[1], $end + 1, -1));
      }
      else {
        $type = $temp[0];
        $terms = trim($temp[1], '()');
      }
    }

    $form['search_type']["type$i"] = array(
      '#title' => '',
      '#type' => 'select',
      '#options' => $types,
      '#default_value' => trim($type)
    );
    $form["fedora_terms$i"] = array(
      '#size' => '24',
      '#type' => 'textfield',
      '#title' => '',
      '#default_value' => stripslashes($terms),
    );
  }

  /**
   * Build solr search form
   * @param type $repeat
   * @param type $pathToSearchTerms
   * @param type $query
   * @return string
   */
  function build_solr_search_form($repeat = NULL, $pathToSearchTerms = NULL, $query = NULL) {
    $types = array();
    $terms = trim(variable_get('islandora_solr_searchterms', 'dc.title ~ Title,dc.subject ~ Subject'));
    $termsArray = preg_split('/[\n]/', $terms);
    foreach ($termsArray as $term) {
      $vals = split('~', $term);
      if (!$vals[1]) {
        $vals[1] = $vals[0];
      }
      $types[trim($vals[0])] = trim($vals[1]);
    }

    $queryArray = NULL;
    if (isset($query)) {
      //A regex like '\\) (OR|AND) \\(' should work, right?  Nope.  Seems that 
      //  preg_split (when given PREG_SPLIT_DELIM_CAPTURE) returns each explicit 
      //  capture group, so for that regex, you'd get only the 'AND' or 'OR'.
      //  If you tried something like '('/(\\) (OR|AND) \\()/')', you'd get the 
      //  'proper' ') AND (' or ') OR (', but also the 'AND' or 'OR' from the 
      //  inner group...  RAGE!...  This turns out to be unused...  Anyway.
      $bool_re = '/(OR|AND)/';
      $queryArray = preg_split($bool_re, $query, 0, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE);
      foreach ($queryArray as $key => $val) {
        if (preg_match($bool_re, $val)) {
          $andOrs[] = trim($val); // that is, just 'OR' or 'AND'
          unset($queryArray[$key]); //remove that from the array
        }
      }
      $queryArray = array_values($queryArray);
    }

    $andOrArray = array(
      'AND' => 'and',
      'OR' => 'or',
    );
    $form = array();

    if (!isset($repeat)) {
      $repeat = variable_get('islandora_solr_search_block_repeat', '3');
    }
    $i = 1;
    $this->_add_field($form, $i, $queryArray[0], $types);

    if ($repeat > 1 && $repeat < 9) { //don't want less then 2 or more then 9
      for ($i = 2; $i < $repeat + 1; $i++) {
        $t = $i - 1;
        $j = $i - 2;
        $andorj = ( isset($andOrs[$j]) ? $andOrs[$j] : 'AND' );

        //$field_and_term = isset($queryArray[$t]) ? explode(':', $queryArray[$t]) : array('', '');
        $form["andor$t"] = array(
          '#title' => '',
          '#type' => 'select',
          '#default_value' => $andorj,
          '#options' => $andOrArray
        );
        $this->_add_field($form, $i, $queryArray[$t], $types);
      }
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('search')
    );
    return $form;
  }

  /**
   * Build simple solr form
   * @global type $islandora_query
   * @return type
   */
  function build_simple_solr_form() {
    // FIXME:  This should actually be checking the settings in the request handler...  Not making assumptions based on name
    $required = FALSE;
    if (variable_get('islandora_solr_search_block_request_handler', '') == 'standard') {
      $required = TRUE;
    }
    global $islandora_query;
    if (!strpos($islandora_query, ':')) {
      $default = $islandora_query;
    }
    // TODO: Fix the above.  The global is no longer valid, and the test is not conclusive.  Should instead test for deftype=dismax.
    $form["islandora_simple_search_query"] = array(
      '#size' => '24%',
      '#type' => 'textfield',
      '#title' => '',
      '#default_value' => $default,
      '#required' => $required,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('search')
    );
    return $form;
  }

  /**
   * Theme solr search form
   * @param type $form
   * @return type
   */
  function theme_solr_search_form($form) {
    if (!isset($repeat)) {
      $repeat = variable_get('islandora_solr_search_block_repeat', '3');
    }

    $output = drupal_render($form['search_type']['type1']);
    $output .= drupal_render($form['fedora_terms1']);
    $output .= drupal_render($form['andor1']) . drupal_render($form['search_type']['type2']);
    $output .= drupal_render($form['fedora_terms2']);
    if ($repeat > 2 && $repeat < 9) {
      for ($i = 3; $i < $repeat + 1; $i++) {
        $t = $i - 1;
        $output .= drupal_render($form["andor$t"]) . drupal_render($form['search_type']["type$i"]);
        $output .= drupal_render($form["fedora_terms$i"]);
      }
    }
    $output .= drupal_render($form['submit']);
    $output .= drupal_render($form);
    return $output;
  }

  /**
   * Merges snippet fields with the rest of the solr results
   * @param Apache Solr Object $results
   * @return array
   */
  function prep_results($results) {
    $rawResponse = $results->getRawResponse();
    $responseArray = json_decode($rawResponse, TRUE);
    $highlights = $responseArray['highlighting'];
    $docs = $responseArray['response']['docs'];

    if (!is_array($highlights)) {
       return $docs;
    }

   // if highights exist, inetegrate them back into the doc array
    $label = variable_get("islandora_solr_snippet_field", "");
    if ($label) {
      $label_parts = explode('~', $label);
      $field_label = trim($label_parts[1]);
      $field_text = trim($label_parts[0]);
    }
    $this->resultFieldArray[$field_text]= $field_label;
    $this->allSubsArray[$field_text]= $field_label;
    foreach ($docs as &$doc) {
      $pid = $doc['PID'];
      unset($doc[$field_text]);
      $snippet = $highlights[$doc['PID']];
      foreach ($snippet as $key => $value) {
        $doc[$field_text] = implode('<br />', $value);
      }
    }
    return $docs;
  }

}

