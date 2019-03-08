<?php

/**
 * @class
 */
class sio__publication extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------.
  /**
   * The default label for this field.
   */
  public static $default_label = 'Feature Publication';

  /**
   * The default description for this field.
   */
  public static $default_description = 'Get publications associated with features';

  /**
   * The default widget for this field.
   */
  public static $default_widget = 'sio__publication_widget';

  /**
   * The default formatter for this field.
   */
  public static $default_formatter = 'sio__publication_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  /**
   * Set this to 'tripal_chado'.
   */
  public static $module = 'tripal_feature_pub';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be.
  /**
   * Changed.
   */
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Indicates the download formats for this field.  The list must be the.

  /**
   * Name of a child class of the TripalFieldDownloader.
   */
  public static $download_formatters = [
    'TripalTabDownloader',
    'TripalCSVDownloader',
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are.
  /**
   * Required for all TripalFields.
   */
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'SIO',
    // The name of the term.
    'term_name' => 'publication',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '000087',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  /**
   * And field_create_instance().
   */
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects.
  /**
   * The user but otherwise provides no data.
   */
  public static $no_data = FALSE;

  /**
   * @see ChadoField::load()
   **/
  public function load($entity) {
    $feature = $entity->chado_record;

    // Get the analysis.
    $analyses = chado_query('SELECT * FROM {analysisfeature} AF 
                             WHERE AF.feature_id = :feature_id', [
      ':feature_id' => $feature->feature_id,
    ])->fetchAll();

    if (!$analyses or empty($analyses)) {
      return;
    }

    $ids = array_map(function ($analysis) {
      return $analysis->analysis_id;
    }, $analyses);

    // Get publications.
    $pubs = chado_query('SELECT * FROM {pub} P
                         INNER JOIN {analysis_pub} AP ON AP.pub_id = P.pub_id
                         WHERE AP.analysis_id IN (:ids)', [
      ':ids' => $ids,
    ])->fetchAll();

    $data = [];
    foreach ($pubs as $pub) {
      $info = $pub;
      $info->tablename = 'pub';
      $info = chado_get_minimal_pub_info($pub);
      $data[] = [
        'value' => $info,
        'record' => $pub,
      ];
    }

    $entity->sio__publication['und'] = $data;
  }

  /**
   * @see ChadoField::query()
   **/
  public function query($query, $condition) {
  }

  /**
   * @see ChadoField::queryOrder()
   **/
  public function queryOrder($query, $order) {
  }

  /**
   * @see ChadoField::elementInfo()
   **/
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
      ],
    ];
  }

}
