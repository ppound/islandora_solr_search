<?php

/**
 * @file
 * Admin functionality
 *
 * Contains all admin related functionality
 */

/**
 * Function to return admin setting form
 * @return array
 */
function islandora_solr_admin_settings($form, &$form_state) {

  //checks for existence of PHP Solr client.
  if (module_exists('apachesolr')) {
    module_load_include('php', 'apachesolr', 'SolrPhpClient/Apache/Solr/Service');
  }
  else {
    module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');
  }
  if (!class_exists('Apache_Solr_Service')) {
    $message = t('This module requires the !url. Please install the client directory in the root directory of this module before continuing.', array('!url' => l(t('Apache Solr php client'), 'http://code.google.com/p/solr-php-client')));
    drupal_set_message(check_plain($message));
    return;
  }

  $handlers['standard'] = 'standard';

  $handler = !empty($form_state['values']['islandora_solr_block_request_handler']) ? $form_state['values']['islandora_solr_block_request_handler'] : variable_get('islandora_solr_block_request_handler', 'standard');
  $solr_url = !empty($form_state['values']['islandora_solr_block_url']) ? $form_state['values']['islandora_solr_block_url'] : variable_get('islandora_solr_block_url', 'localhost:8080/solr');
  $solr_avail = solr_available($solr_url);
  if ($solr_avail) {
    $handlers = get_handlers($solr_url);
  }

  $dismax_allowed = check_dismax($solr_url, $handler);
  $confirmation_message = ($solr_avail ? '<img src="' . url('misc/watchdog-ok.png') . '"/>'
          . t('Successfully connected to Solr server at !islandora_solr_block_url', array('!islandora_solr_block_url' => $solr_url)) : '<img src="'
          . url('misc/watchdog-error.png') . '"/> '
          . t('Unable to connect to Solr server at !islandora_solr_block_url</p>', array('!islandora_solr_block_url' => $solr_url)));

  $form = array();

  $form['solr_ajax_wrapper'] = array(
    '#prefix' => '<div id="solr-url">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
  );

  $form['solr_ajax_wrapper']['islandora_solr_block_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Solr url'),
    '#size' => 40,
    '#weight' => -1,
    '#description' => t('The url of the Solr installation.  Defaults to localhost:8080/solr.'),
    '#default_value' => $solr_url,
    '#ajax' => array(
      'callback' => 'update_solr_url_div',
      'wrapper' => 'solr-url',
      'effect' => 'fade',
      'event' => 'blur',
      'progress' => array('type' => 'none'), // don't show progress throbber
    ),
  );

  $form['solr_ajax_wrapper']['refresh_page'] = array(
    '#type' => 'submit',
    '#value' => t('Test connection'),
    '#attributes' => array('class' => array('refresh-button')),
    '#submit' => array('solr_admin_refresh'),
  );

  $form['solr_ajax_wrapper']['infobox'] = array(
    '#type' => 'item',
    '#markup' => $confirmation_message,
  );

  $form['solr_ajax_wrapper']['islandora_solr_block_request_handler'] = array(
    '#type' => 'select',
    '#title' => t('Request Handler'),
    '#options' => $handlers,
    '#description' => t('Request handlers as defined by !config', array('!config' => l(t('solrconfig.xml'), 'http://wiki.apache.org/solr/SolrConfigXml'))),
    '#default_value' => $handler,
    '#ajax' => array(
      'callback' => 'update_solr_url_div',
      'wrapper' => 'solr-url',
      'effect' => 'fade',
      'event' => 'change',
      'progress' => array('type' => 'none'), // don't show progress throbber
    ),
  );


  // display profiles
  $form['display_profiles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display profiles'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  // primary displays
  $form['display_profiles']['islandora_solr_primary_display_table'] = array(
    '#type' => 'item',
    '#title' => t('Primary Display Profiles'),
    '#description' => t('Preferred normal display profile for search results.  These may be provided by third-party modules.'),
    '#tree' => TRUE, // this attribute is important to return the submitted values in a deeper nested arrays in
    '#theme' => 'islandora_solr_admin_primary_display',
  );
  
  
  // get the table settings
  $primary_display_array = variable_get('islandora_solr_primary_display_table', array());
  dsm(($primary_display_array));
  // get all defined primary displays
  $profiles = module_invoke_all("islandora_solr_primary_display");
  dsm($profiles);
  if (!empty($profiles)) {

    $profiles_sorted = array();
    // if the table settings are set (weight), we change the order of the table rows.
    if (isset($primary_display_array['weight'])) {
      // new variable
      $weight = $primary_display_array['weight'];

      // First sort by value and then sort equal values by key: http://stackoverflow.com/a/6611077/477949
      array_multisort(array_values($weight), SORT_ASC, array_keys($weight), SORT_ASC, $weight);

      // Add all previously existing profiles with a weight...
      foreach (array_intersect_assoc($profiles, $weight) as $key => $value) {
        $profiles_sorted[$key] = $value;
      }
	  
      //Account for new profiles.
      foreach (array_diff_assoc($profiles, $profiles_sorted) as $key => $value) {
        $profiles_sorted[$key] = $value;
      }
    }
    // or else use the default
    else {
      // sort alphabetically with default on top
      ksort($profiles); // only apply when there's no sort variable available.
      $profiles_sorted = $profiles;
    }


    // table loop
    foreach ($profiles_sorted as $machine_name => $profile) {

      $options[$machine_name] = '';

      $form['display_profiles']['islandora_solr_primary_display_table']['name'][$machine_name] = array(
        '#type' => 'item',
        '#markup' => $profile['name'],
      );
      $form['display_profiles']['islandora_solr_primary_display_table']['machine_name'][$machine_name] = array(
        '#type' => 'item',
        '#markup' => $machine_name,
      );
      $form['display_profiles']['islandora_solr_primary_display_table']['weight'][$machine_name] = array(
        '#type' => 'weight',
//        '#default_value' => ($primary_display_array['weight'][$machine_name]) ? $primary_display_array['weight'][$machine_name] : 0,
        '#default_value' => 0, // if no displays are set 'weight' is not set. Need fixing
        '#attributes' => array('class' => 'solr-weight'),
      );
      $form['display_profiles']['islandora_solr_primary_display_table']['configuration'][$machine_name] = array(
        '#type' => 'item',
        '#markup' => (isset($profile['configuration']) AND $profile['configuration'] != '') ? l(t('configure'), $profile['configuration']) : '',
      );
    }

    $form['display_profiles']['islandora_solr_primary_display_table']['default'] = array(
      '#type' => 'radios',
      '#options' => $options,
      '#default_value' => variable_get('islandora_solr_primary_display', 'default'),
    );
    $form['display_profiles']['islandora_solr_primary_display_table']['enabled'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => (!empty($primary_display_array)) ? $primary_display_array['enabled'] : $primary_display_array, // $primary_display_array['enabled'][$machine_name]
    );
  }

  
  // secondary profiles
  $profiles = module_invoke_all("islandora_solr_secondary_display");
  ksort($profiles);
  foreach ($profiles as $machine_name => $profile) {
    $islandora_solr_secondary_display_options[$machine_name] = $profile['name'];
  }
  
  if (!empty($islandora_solr_secondary_display_options)) {
    $form['display_profiles']['islandora_solr_secondary_display'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Secondary Display Profiles'),
      '#options' => $islandora_solr_secondary_display_options,
      '#default_value' => variable_get('islandora_solr_secondary_display', array()),
      '#description' => "Enabled secondary output/download types for search results.",
    );
  }



  // default display settings
  $form['default_display_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default display settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['default_display_settings']['islandora_solr_result_fields'] = array(
    '#type' => 'textarea',
    '#title' => t('Labels for Returned Fields'),
    '#description' => t('Set labels for fields returned from query.
      Enter terms on separate lines using the following pattern: field [tilde] preferred label. ie <strong>dc.title ~ Title</strong>'),
    '#default_value' => variable_get('islandora_solr_result_fields', 'dc.subject ~ Subject,dc.type ~ Type'),
    '#wysiwyg' => FALSE,
  );
  $form['default_display_settings']['islandora_solr_limit_result_fields'] = array(
    '#type' => 'checkbox',
    '#title' => t('Limit results to fields listed above?'),
    '#return_value' => 1,
    '#default_value' => variable_get('islandora_solr_limit_result_fields', 0),
    '#description' => t('If checked, results displayed will be limited to the fields specified above.<br/><strong>Note:</strong> Some display profiles may not honour this value.'),
  );
  $form['default_display_settings']['islandora_solr_snippet_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Snippet Field'),
    '#default_value' => variable_get('islandora_solr_snippet_field', ''),
    '#description' => t("If a match is found on this field, a snippet of text will be returned, with the search term highlighted.<br />
      An optional friendly label may inserted using the following pattern <strong>dsm.Text ~ Full Text</strong><br />
      Use an asterisk (<strong>*</strong>) for all fields, leave blank to disable highlighting.<br />
      <strong>Note:</strong><em> This feature is not supported by all display profiles.</em> "),
    '#wysiwyg' => FALSE,
  );
  $form['default_display_settings']['islandora_solr_num_of_results'] = array(
    '#type' => 'textfield',
    '#title' => t('Results per page'),
    '#size' => 5,
    '#description' => t('The number of results to show per page.'),
    '#default_value' => variable_get('islandora_solr_num_of_results', '20'),
  );

  

  // facet settings
  $form['facet_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facet settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['facet_settings']['islandora_solr_facets'] = array(
    '#type' => 'textarea',
    '#title' => t('Facet Fields'),
    '#default_value' => variable_get('islandora_solr_facets', ''),
    '#description' => t("Indicate which fields will appear in the <strong>Islandora Facet Block</strong>.<br />
      Enter terms on separate lines using the following pattern: field [tilde] preferred label. ie <strong>dc.title ~ Title</strong> "),
    '#default_value' => variable_get('islandora_solr_facets', 'dc.subject ~ Subject,dc.type ~ Type'),
    '#wysiwyg' => FALSE,
    '#default_value' => variable_get('islandora_solr_facets', 'dc.subject ~ Subject,dc.type ~ Type')
  );
  $form['facet_settings']['islandora_solr_facet_min_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum limit'),
    '#size' => 5,
    '#description' => t('Minimum results required to display a facet'),
    '#default_value' => variable_get('islandora_solr_facet_min_limit', '2'),
  );
  $form['facet_settings']['islandora_solr_facet_soft_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Soft limit'),
    '#size' => 5,
    '#description' => t('The number which should be shown initially. If there are more, then the a "Show more" button will allow the rest up to the value below to be displayed. Use 0 to disable.'),
    '#default_value' => variable_get('islandora_solr_facet_soft_limit', '0'),
  );
  $form['facet_settings']['islandora_solr_facet_max_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum limit'),
    '#size' => 5,
    '#description' => t('Set the maximum number of terms that should be returned to the user.<br />
      For example, if there are 100 possible subjects in a faceted result, you may wish to return only the top 10.'),
    '#default_value' => variable_get('islandora_solr_facet_max_limit', '20'),
  );
  $form['facet_settings']['islandora_facet_display_table'] = array();  
  
  
  // advanced search block
  $form['advanced_search_block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced search block'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['advanced_search_block']['islandora_solr_searchterms'] = array(
    '#type' => 'textarea',
    '#title' => t('Search Terms'),
    '#description' => t("Indicate what fields should appear in the dropdown menu of terms for the <strong>Solr Advanced Search Block</strong>.<br />
      Enter terms on separate lines using the following pattern: field [tilde] preferred label. ie <strong>dc.title ~ Title</strong><br />
      Review the schema.xml to see what terms are available. A list of the fields available when using the schema.xml packaged with the Islandora Solr
      module is available in Islandora's online documentation."),
    '#default_value' => variable_get('islandora_solr_searchterms', 'dc.title ~ Title'),
    '#wysiwyg' => FALSE,
  );
  $form['advanced_search_block']['islandora_solr_search_block_repeat'] = array(
    '#type' => 'textfield',
    '#title' => t('Search Field Repetition'),
    '#size' => 5,
    '#description' => t('The number of fields you would like to have appear in the <strong>Solr Advanced Search Block</strong>'),
    '#default_value' => variable_get('islandora_solr_search_block_repeat', '3'),
  );


  
  // query defaults
  $form['query_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Query defaults'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['query_defaults']['islandora_solr_namespace_restriction'] = array(
    '#type' => 'textfield',
    '#title' => t('Limit results to namespace'),
    '#size' => 5,
    '#default_value' => variable_get('islandora_solr_namespace_restriction', ''),
    '#description' => t("Enter a namespace ie 'demo' to restrict results to PIDs within that namespace."),
  );
  $form['query_defaults']['islandora_solr_base_query'] = array(
    '#type' => 'textfield',
    '#title' => t('Solr Default Query'),
    '#size' => 30,
    '#description' => t('A default query to use to browse solr results when no explicit user query is set.
      Setting a useful default query allows users to use solr to browse, without entering a query themselves.
      May be used in conjunction with a background filter, below.<br>
      Consider <strong><em>timestamp:[* TO NOW]</em></strong> or <strong><em>*:*</em></strong><br/>'),
    '#default_value' => variable_get('islandora_solr_base_query', 'timestamp:[* TO NOW]'),
  );
  $form['query_defaults']['islandora_solr_base_filter'] = array(
    '#type' => 'textarea',
    '#title' => t('Solr Base Filter'),
    '#description' => t('Some base filters to append to all user queries -- may be used to filter results and to facilitate null-query browsing. Enter one per line. <br/>
      These filters will be applied to all queries in addition to any user-selected facet filters'),
    '#default_value' => variable_get('islandora_solr_base_filter', ''),
    '#wysiwyg' => FALSE,
  );
  $form['query_defaults']['sort_fields'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['query_defaults']['sort_fields']['islandora_solr_sort_field'] = array(
    '#type' => 'textfield', '#title' => t('Sort field'),
    '#size' => 20,
    '#description' => t('Results will be sorted on this field. If left blank will default to <em>score</em>.'),
    '#default_value' => variable_get('islandora_solr_sort_field', ''),
  );

  $form['query_defaults']['sort_fields']['islandora_solr_sort_order'] = array(
    '#type' => 'radios',
    '#title' => t('Sort Order'),
    '#options' => array('asc' => t('Ascending'), 'desc' => t('Decending')),
    '#default_value' => 'desc',
  );
  
  // debug mode
  $form['islandora_solr_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug Mode?'),
    '#return_value' => 1,
    '#default_value' => variable_get('islandora_solr_debug_mode', 0),
    '#description' => t('Dumps solr query to the screen for testing. Warning: if you have the Drupal Apache Solr module enabled alongside this one then the debug function will not work.'),
  );
  
  
  //Notice: Undefined index: #options in expand_checkboxes() (line 1930 of /Applications/MAMP/htdocs/drupal-6.22/includes/form.inc).
  $form['dismax_allowed'] = array(
    '#type' => 'hidden',
    '#value' => $dismax_allowed,
  );
  
  
  // actions
  $form['actions'] = array(
    '#type' => 'actions'
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Solr Configuration'),
    '#weight' => 0,
    '#submit' => 'solr_settings_form_submit',
  );
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#weight' => 1,
  );

  if (!empty($_POST) && form_get_errors()) {
    drupal_set_message(t('The settings have not been saved because of the errors.'), 'error');
  }

  return $form;
}


/**
 * theme_islandora_solr_admin_primary_display()
 */
function theme_islandora_solr_admin_primary_display($variables) {
  // set form
  $form = $variables['form'];

  $default = variable_get('islandora_solr_primary_display', 'default');
  $rows = array();
  
  foreach ($form['name'] as $key => $element) {
    // Do not take form control structures.
    if (is_array($element) && element_child($key)) {
      // disable checkbox for the default display, because it cannot be disabled.
      if ($key == $default) {
        $form['enabled'][$key]['#attributes']['disabled'] = 'disabled';
        $form['enabled'][$key]['#checked'] = TRUE;
      }

      $row = array();
      $row[] = array('data' => drupal_render($form['default'][$key]));
      $row[] = array('data' => drupal_render($form['enabled'][$key]));
      $row[] = array('data' => drupal_render($form['name'][$key]), 'title' => $key);
      $row[] = array('data' => drupal_render($form['machine_name'][$key]));
      $row[] = array('data' => drupal_render($form['weight'][$key]), 'class' => 'solr-weight');
      $row[] = array('data' => drupal_render($form['configuration'][$key]));

      // add to rows
      $rows[] = array('data' => $row, 'class' => array('draggable'));
     }
  }

  // Individual table headers.
  // default | enabled | name | Machine readable name |  weight | configuration
  $header = array();
  $header[] = array('data' => t('Default'));
  $header[] = array('data' => t('Enabled'));
  $header[] = array('data' => t('Name'));
  $header[] = array('data' => t('Machine readable name'));
  $header[] = array('data' => t('Weight'));
  $header[] = array('data' => t('Configuration'));
  
  // render form
  $output = '';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'solr-primary-display-table')));
  $output .= drupal_render_children($form);
  
  // add tabledrag features
  drupal_add_tabledrag('solr-primary-display-table', 'order', 'sibling', 'solr-weight');
    
  return $output;
}

/**
 *
 * @param String $solr_url
 * @return boolean
 *
 * Checks availability of Solr installation
 *
 */
function solr_available($solr_url) {
  $handle = curl_init($solr_url);
  curl_setopt($handle, CURLOPT_RETURNTRANSFER, TRUE);
  $response = curl_exec($handle);
  $httpCode = curl_getinfo($handle, CURLINFO_HTTP_CODE);
  $retval = TRUE;
  if ($httpCode % 100 == 4) { //FIXME:  Should probably deal with 500 errors as well?
    $retval = FALSE;
  }
  curl_close($handle);
  return $retval;
}

/**
 * get handlers??
 * @param String $solr_url
 * @return array
 */
function get_handlers($solr_url) {
  $url = $solr_url . '/admin/file/?file=solrconfig.xml';
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_URL, $url);
  $xml = simplexml_load_string(curl_exec($ch));
  foreach ($xml->requestHandler as $handler) {
    if ($handler['class'] == 'solr.SearchHandler') {
      $handlerName = (string) $handler['name'];
      $slashed = preg_match('/^\//', $handlerName);
      if ($handlerName != 'dismax' && $handlerName != 'partitioned' & !$slashed) {
        $handlers[$handlerName] = $handlerName;
      }
    }
  }
  return $handlers;
}

/**
 * Checks url validity and refreshes requestHandler dropdown list
 */
function update_solr_url_div($form, $form_state) {
  return $form['solr_ajax_wrapper']; // man, this is easy :)
}

/**
 * Solr admin refresh
 * @param type $form
 * @param array $form_state
 */
function solr_admin_refresh($form, &$form_state) {
  unset($form_state['submit_handlers']);
  form_execute_handlers('submit', $form, $form_state);
  $form_state['rebuild'] = TRUE;
}

/**
 * Form validator
 * @param array $form
 * @param array $form_state
 */
function islandora_solr_admin_settings_validate($form, &$form_state) {
  $form_state['values']['islandora_solr_namespace_restriction'] = preg_replace('/:$/', '', $form_state['values']['islandora_solr_namespace_restriction']);
}

/**
 * Solr settings from submit
 * @param array $form
 * @param array $form_state
 * @return NULL
 */
function solr_settings_form_submit($form, &$form_state) {
  // old_default variable
  $old_default = variable_get('islandora_solr_primary_display', 'default');
  // new_default variable
  $new_default = $form_state['values']['islandora_solr_primary_display_table']['default'];
  // make default enabled
  $form_state['values']['islandora_solr_primary_display_table']['enabled'][$new_default] = $new_default;
  $form_state['values']['islandora_solr_primary_display_table']['enabled'][$old_default] = $old_default;
  variable_set('islandora_solr_primary_display', $new_default);

  if ($form_state['clicked_button']['#id'] != 'edit-submit') {
    $form_state['rebuild'] = TRUE;
    return;
  }
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  // Exclude unnecessary elements.
  unset($form_state['values']['submit'], $form_state['values']['reset'], $form_state['values']['form_id'], $form_state['values']['op'], $form_state['values']['form_token'], $form_state['values']['form_build_id']);

  foreach ($form_state['values'] as $key => $value) {
    if ($op == t('Reset to defaults')) {
      variable_del($key);
    }
    else {
      if (is_array($value) && isset($form_state['values']['array_filter'])) {
        $value = array_keys(array_filter($value));
      }
      variable_set($key, $value);
    }
  }
  if ($op == t('Reset to defaults')) {
    drupal_set_message(t('The configuration options have been reset to their default values.'));
  }
  else {
    drupal_set_message(t('The solr configuration options have been saved.'));
  }

  cache_clear_all();
  drupal_theme_rebuild();
}




/**
 * Checks for dismax
 * @param type $solr_url
 * @param type $selected_handler
 * @return boolean 
 */
function check_dismax($solr_url, $selected_handler) {
  $dismax = FALSE;
  $url = $solr_url . '/admin/file/?file=solrconfig.xml';
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_URL, $url);
  $xml = simplexml_load_string(curl_exec($ch));
  foreach ($xml->requestHandler as $handler) {
    if ($handler['class'] == 'solr.SearchHandler') {
      if ((string) $handler['name'] == $selected_handler) {
        foreach ($handler->lst as $first) {
          foreach ($first->str as $str) {
            if ($str['name'] == 'qf') {
              $dismax = TRUE;
            }
          }
        }
      }
    }
  }
  return $dismax;
}
