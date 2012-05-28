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
      'file' => 'islandora_solr.block.inc',
      'class' => NULL,
      'function' => NULL,
      'form' => 'islandora_solr_advanced_search_form',
    ),
    'simple' => array(
      'name' => t('Islandora Simple Search'),
      'module' => 'islandora_solr',
      'file' => 'islandora_solr.block.inc',
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
      'file' => 'islandora_solr.block.inc',
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
function islandora_solr_simple_search_form($form, &$form_state) {

  $form["islandora_simple_search_query"] = array(
    '#size' => '24',
    '#type' => 'textfield',
    '#title' => '',
    '#default_value' => '', // @TODO: should this be the searched value?
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
  $search_string = $form_state['values']['islandora_simple_search_query'];
  $search_string = str_replace('/', '~slsh~', $search_string); //replace the slash so url doesn't break
  
  // set query
  $query = array('type' => 'dismax');
  
  drupal_goto('islandora/search/' . $search_string, array('query' => $query));
}



/**
 * islandora solr search block form
 * @global type $_islandora_solr_queryclass
 * @return type
 */
function islandora_solr_advanced_search_form($form, &$form_state) {
  global $_islandora_solr_queryclass;
  islandora_solr_init(); // @TODO: figure out why init needs to be called

  // include common.inc
  module_load_include('inc', 'islandora_solr', 'includes/common');
  
  // check if values are already set
  if (isset($form_state['triggering_element'])) {
    
    // unset terms from input array
    // http://drupal.stackexchange.com/questions/14855/how-do-i-dynamically-fill-a-textfield-with-ajax/16576#16576
    unset($form_state['input']['terms']);
    // check for add
    if ($form_state['triggering_element']['#value'] == '+') {
      $form_state['values']['terms'][] = array();  
    }
    // check for remove
    elseif ($form_state['triggering_element']['#value'] == '-') {
      $field = $form_state['triggering_element']['#field'];
      array_splice($form_state['values']['terms'], $field, 1);
    }
    
    $values = $form_state['values'];
  }
  else {
    //Need at least one term to draw the search box.
    $values = array(
      'terms' => array(''),
    );
  }
  $terms = array(
    '#type' => 'markup',
    '#prefix' => '<div id="islandora-solr-advanced-terms">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  // loop over each term
  foreach ($values['terms'] as $i => $value) {
    $term = array(
      '#tree' => TRUE,
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
    $term['field'] = array(
      '#title' => t('Field'),
      '#type' => 'select',
      '#default_value' => isset($value['field']) ? $value['field'] : 'dc.title',
      '#options' => islandora_build_substitution_list(variable_get('islandora_solr_searchterms', 'dc.title ~ Title,dc.subject ~ Subject')),
    );
    $term['search'] = array(
      '#title' => t('Search terms'),
      '#type' => 'textfield',
      '#size'=> 20,
      '#default_value' => isset($value['search']) ? $value['search'] : '',
    );
    $term['hidden_submit'] = array(// Used for when the user presses enter on the search field.
      '#type' => 'submit',
      '#value' => t('Search'),
      '#attributes' => array('style' => 'visibility:hidden;position:fixed;top:-1000px;right:-1000px;')
    );
    $term['add'] = array(
      '#type' => 'button',
      '#value' => '+',
      '#attributes' => array('title' => t('Add field')),
      '#name' => 'add-field-' . $i,
      '#ajax' => array(
        'callback' => '_islandora_solr_advanced_search_terms',
        'wrapper' => 'islandora-solr-advanced-terms',
        'method' => 'replace',
        'effect' => 'fade',
        'progress' => array('type' => 'none'),
      )
    );
    if (count($values['terms']) > 1) {
      $term['remove'] = array(
        '#type' => 'button',
        '#field' => $i,
        '#value' => '-',
        '#attributes' => array('title' => t('Remove field')),
        '#name' => 'remove-field-' . $i,
        '#ajax' => array(
          'callback' => '_islandora_solr_advanced_search_terms',
          'wrapper' => 'islandora-solr-advanced-terms',
          'method' => 'replace',
          'effect' => 'fade',
          'progress' => array('type' => 'none'),
        )
      );
      if ((variable_get('islandora_solr_search_boolean', 'user') == 'user') AND ((count($values['terms']) -1) != $i)) {
        $term['boolean'] = array(
          '#type' => 'select',
          '#prefix' => '<div>',
          '#suffix' => '</div>',
          '#default_value' => isset($value['boolean']) ? $value['boolean'] : 'AND',
          '#options' => array(
            'AND' => 'AND',
            'OR' => 'OR',
            'NOT' => 'NOT'
          )
        );
      }
    }
    $terms[] = $term;
  }
  // set form
  $form = array();
  // add terms
  $form['terms'] = $terms;
  // add controls
  $form['controls'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="islandora-solr-advanced-controls">',
    '#suffix' => '</div>',
  );
  if (variable_get('islandora_solr_search_sortby', 1) == 1) {
    $form['controls']['sort'] = array(
      '#type' => 'select',
      '#title' => t('Sort By'),
      '#default_value' => (isset($values['sort']) ? $values['sort'] : 'score'),
      '#options' => islandora_build_substitution_list(variable_get('islandora_solr_search_sortby_terms', 'score ~ Relevance, dc.title ~ Title, dc.date ~ Date Issued')),
    );
    $form['controls']['order'] = array(    
      '#type' => 'select',
      '#title' => t('Order'),
      '#default_value' => (isset($values['order']) ? $values['order'] : 'desc'),
      '#options' => array(
        'desc' => t('Descending'),
        'asc' => t('Ascending'),
      ),
    );
  }
  $form['controls']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search')
  );
  return $form;
}



/**
 * Callback for advanced search form
 * @param type $form
 * @param type $form_state
 * @return type 
 */
function _islandora_solr_advanced_search_terms($form, &$form_state) {
  return $form['terms'];
}








/**
 * Submit hook for the quick search form.
 * 
 * @param array $form
 * @param array $form_state 
 */
function islandora_solr_advanced_search_form_submit($form, &$form_state) {
  
  // Collect query values
  $query_array = array();
  foreach ($form_state['values']['terms'] as $term) {
    // set field
    $field = $term['field'];
    // set search value
    $search = trim($term['search']);
    //replace the slash so url doesn't break
    $search = str_replace('/', '~slsh~', $search);
    // set boolean
    $boolean = (isset($term['boolean'])) ? $term['boolean'] : variable_get('islandora_solr_search_boolean', 'user');
    
    // add query
    if (!empty($search)) {
      $query_array[] = array(
        'search' => "$field:(\"$search\")",
        'boolean' => $boolean
      );
    }
  }

  // create query
  $query = '';
  $i = 0;
  foreach ($query_array as $term) {
    $query .= $term['search'];
    if (count($query_array) -1 != $i) {
      $query .= ' ' . $term['boolean'] . ' ';
    }
    $i++;
  }
  
  // check if query is empty
  if (empty($query)) {
    $query = '*:*';
  }
  
  // set sort parameters
  $params = array();
  if (variable_get('islandora_solr_search_sortby', 1) == 1) {
    $sort = $form_state['values']['sort'];
    $order = $form_state['values']['order'];
    
    $params = array('query' => array('sort' => $sort . ' ' . $order));
  }

  drupal_goto('islandora/search/' . $query, $params);
}




/**
 * @return display switch
 */
function islandora_solr_display() {
  global $_islandora_solr_queryclass;
  
  // set variables
  $output = '';
  
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
        $output = theme('item_list', array('items' => $list_items, 'title' => NULL, 'type' => 'ul', 'attributes' => array('class' => 'islandora-solr-display')));
      }
    }
  }
  return $output;
}
