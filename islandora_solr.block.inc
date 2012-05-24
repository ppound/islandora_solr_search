<?php

/**
* @file
* Contains all blocks related functionality
*/

/**
 * Implements hook_islandora_solr_query_blocks()
 */
function islandora_solr_islandora_solr_query_blocks() {
  return array(
    // 'machine_name' => array(
    //   'name' => 'Human Readable Name',
    //   'module' => 'module_name',
    //   'file' => 'FileName.inc',
    //   'class' => 'ClassName',
    //   'function' => 'method_name',
    //   'form' => 'form_function_name',
    //  ),
    //
    // Note: As in the examples below, it is valid to specify
    // *either* a class and method *or* a form.  The latter implies no
    // class needs to be instantiated.
    //

    'advanced' => array(
      'name' => t('Islandora Advanced Search'),
      'module' => 'islandora_solr',
      'file' => 'islandora_solr.module',
      'class' => NULL,
      'function' => NULL,
      'form' => 'islandora_solr_block_form',
    ),
    'simple' => array(
      'name' => t('Islandora Simple Search'),
      'module' => 'islandora_solr',
      'file' => 'islandora_solr.module',
      'class' => NULL,
      'function' => NULL,
      'form' => 'islandora_solr_simple_search_form',
    ),
    'basic_facets' => array(
      'name' => t('Islandora Facets'),
      'module' => 'islandora_solr',
      'file' => 'IslandoraSolrResults.inc',
      'class' => 'IslandoraSolrResults',
      'function' => 'displayFacets',
      'form' => NULL,
    ),
    'current_query' => array(
      'name' => t('Islandora Query'),
      'module' => 'islandora_solr',
      'file' => 'IslandoraSolrResults.inc',
      'class' => 'IslandoraSolrResults',
      'function' => 'currentQuery',
      'form' => NULL,
    ),
    'display_switch' => array(
      'name' => 'Islandora Displays',
      'module' => 'islandora_solr',
      'file' => 'islandora_solr.module',
      'class' => NULL,
      'function' => 'islandora_solr_display',
      'form' => NULL,
    ),
  );
}


/**
 * Implementation of hook_block_info().
 */
function islandora_solr_block_info() {
  // set variable
  $blocks = array();
  
  // return a list of block descriptions for block administration
  $solr_blocks = module_invoke_all("islandora_solr_query_blocks");
  foreach ($solr_blocks as $name => $block ) {
    $blocks[$name] = array(
      'info' => t($block['name']),
      'cache' => DRUPAL_NO_CACHE,
    );
  }
  return $blocks;
}




/**
 * Implementation of hook_block_view().
 */
function islandora_solr_block_view($block_name = '') {
  // set variable
  $block = array();
  
  // get global class
  global $_islandora_solr_queryclass;
  
  // all hook_init()
  islandora_solr_init(); // @TODO: should we really call islandora_solr_init() again ?
  
  // get solr blocks
  $solr_blocks = module_invoke_all("islandora_solr_query_blocks");

  // if the block that is called is defined
  if (!empty($solr_blocks[$block_name])) {

    // set block title
    $block['subject'] = $solr_blocks[$block_name]['name'];

    // Include the file from which the block originates.
    require_once( drupal_get_path('module', $solr_blocks[$block_name]['module']) . '/' . $solr_blocks[$block_name]['file'] );

    // if a class is present, instantiate it and proceed.
    // The variable $_islandora_solr_queryclass (the IslandoraSolrQueryProcessor, containing
    // the Solr search result), is fed as an argument.
    if (!empty($solr_blocks[$block_name]['class'])) {
      $displayClass = new $solr_blocks[$block_name]['class']();
      $block_function = $solr_blocks[$block_name]['function'];
      if (method_exists($displayClass, $block_function)) {
        // set content
        $block['content'] = $displayClass->$block_function($_islandora_solr_queryclass);
      }
      // otherwise, simply load the form.
    }
    elseif (!empty($solr_blocks[$block_name]['form'])) {
      // set content
      $block['content'] = drupal_get_form($solr_blocks[$block_name]['form']);
    }
    // else if only a function is given (no class, no form)
    elseif (!empty($solr_blocks[$block_name]['function'])) {
      $function = $solr_blocks[$block_name]['function'];
      // set content
      $block['content'] = $function();
    }
  }
  return $block;
}









/**
 * islandora solr simple search form
 * @return type
 */
function islandora_solr_simple_search_form() {
  // FIXME:  This should actually be checking the settings in the request handler...  Not making assumptions based on name
  $required = FALSE;
  if (variable_get('islandora_solr_request_handler', '') == 'standard') {
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
 * islandora solr simple search form submit
 * @param type $form
 * @param type $form_state
 */
function islandora_solr_simple_search_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $searchString = $form_state['values']['islandora_simple_search_query'];
  $searchString = str_replace('/', '~slsh~', $searchString); //replace the slash so url doesn't break
  
  // set query
  $query = array('type' => 'dismax');
  
  drupal_goto("islandora/search/$searchString", $query);
}




/**
 * islandora solr search block form
 * @global type $_islandora_solr_queryclass
 * @return type
 */
function islandora_solr_block_form() {
  global $_islandora_solr_queryclass;
  islandora_solr_init();

  $repeat = NULL;
  $query = $_islandora_solr_queryclass->solrQuery;
  
  
  $types = array();
    $terms = trim(variable_get('islandora_solr_searchterms', 'dc.title ~ Title,dc.subject ~ Subject'));
    $termsArray = preg_split('/[\n]/', $terms);
    foreach ($termsArray as $term) {
      $vals = explode('~', $term);
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
      $repeat = variable_get('islandora_solr_block_repeat', '3');
    }
    $i = 1;
    _add_field($form, $i, $queryArray[0], $types);

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
        _add_field($form, $i, $queryArray[$t], $types);
      }
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('search')
    );
    return $form;
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
        //$terms = trim($temp[1], '()');
        $terms = '';
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
 * theme islandora solr search block form
 * @global type $_islandora_solr_queryclass
 * @param type $form
 * @return type
 */
function theme_islandora_solr_block_form($form) {
  global $_islandora_solr_queryclass;
  islandora_solr_init();

    if (!isset($repeat)) {
      $repeat = variable_get('islandora_solr_block_repeat', '3');
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
 * islandora solr search block form validate
 * @param type $form
 * @param type $form_state
 */
function islandora_solr_block_form_validate($form, &$form_state) {

  $repeat = variable_get('islandora_solr_block_repeat', '3');
  $found = FALSE;
  for ($fieldNum = 1; $fieldNum <= $repeat; $fieldNum++) {
    if (!empty($form_state['values']["fedora_terms$fieldNum"])) {
      if ($form_state['values']["fedora_terms$fieldNum"] == '*') {
       unset($form_state['values']["fedora_terms$fieldNum"]);
       continue;
      }
      $found = TRUE;
    }
  }
  if (!$found) {
    form_set_error('edit_fedora_terms1', t('Please enter search term.'));
  }
}

/**
 * islandora solr search block form submit
 * @global type $_islandora_solr_queryclass
 * @param type $form
 * @param type $form_state
 */
function islandora_solr_block_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  global $_islandora_solr_queryclass;

  $type_id = trim($form_state['values']['type']);
  $repeat = variable_get('islandora_solr_block_repeat', '3');
  $fedora_terms = array();
  $types = array();
  $booleans = array();

  for ($fieldNum = 1; $fieldNum <= $repeat; $fieldNum++) {
    if ($form_state['values']["fedora_terms$fieldNum"]) {
      $types[] = trim($form_state['values']["type$fieldNum"]);
      $fedora_terms[] = lesser_escape(trim($form_state['values']["fedora_terms$fieldNum"]));
    }
    $next = $fieldNum + 1;
    if ($form_state['values']["fedora_terms$next"] && $form_state['values']["fedora_terms$fieldNum"]) {
      $booleans[] = trim($form_state['values']["andor$fieldNum"]);
    }
  }

  for ($iteration = 0; $iteration < count($fedora_terms); $iteration++) {
    //FIXME (minor): t() should be changed to format_string() in this instance, in Drupal 7
    //  The string isn't really translatable, just templated.
    $searchString .= t("!field:(!query) !bool ", array(
        '!field' => $types[$iteration], 
        '!query' => $fedora_terms[$iteration],
        '!bool' => $booleans[$iteration]
    ));
  }
  $searchString = trim($searchString);
  $searchString = str_replace('/', '~slsh~', $searchString); //replace the slash so url doesn't break
  drupal_goto("islandora/search/$searchString");
}

/**
 * @return display switch
 */
function islandora_solr_display() {
  global $_islandora_solr_queryclass;
  
  // set variables
  $output = FALSE;
  
  // check if the page is dealing with search results
  if (class_exists('IslandoraSolrResults')) {
  
    // parameters set in url
    $params = isset($_islandora_solr_queryclass->internalSolrParams) ? $_islandora_solr_queryclass->internalSolrParams : array();
    // set path
    $path = SOLR_SEARCH_PATH . '/' . $_islandora_solr_queryclass->solrQuery;
  
    // get profiles
    $profiles = module_invoke_all("islandora_solr_primary_display");
    // get the table settings
    $primary_display_array = variable_get('islandora_solr_primary_display_table', array());
    
    if (!empty($primary_display_array)) {

      $weight = $primary_display_array['weight'];
      $enabled = $primary_display_array['enabled'];
      $default = $primary_display_array['default'];

      // sort by weight
      array_multisort(array_values($weight), SORT_ASC, array_keys($weight), SORT_ASC, $weight);

      // set variables
      $list_items = array();

      foreach ($weight as $key => $value) {
        if ($enabled[$key] !== 0) {

          // current display
          if (isset($params['display'])) {
            $current_display = $params['display'];
          }
          else {
            $current_display = $default;
          }
          
          // title
          $display_name = $profiles[$key]['name'];
          
          // query
          $query = array_merge($params, array('display' => $key));
          
          // set attributes
          $attr = array();
          // set class
          if ($key == $current_display) {
            $attr['class'] = 'active';
          }
          // nofollow
          $attr['rel'] = 'nofollow';
          // title
          $attr['title'] = $display_name;
          // url
          $attr['href'] = url($path, array('query' => $query));
          
          // create link
          // we're not using l() because of active classes: http://drupal.org/node/41595
          $item = '<a' . drupal_attributes($attr) . '>' . $display_name . '</a>';
          
          // add link to list
          $list_items[] = $item;
        }
      }
      // theme item list (if more than one displays are available)
      if (count($list_items) > 1) {
        $output = theme('item_list', $list_items, NULL, "ul", array('class' => 'islandora-solr-search-display'));
      }
    }
  }
  return $output;
}
