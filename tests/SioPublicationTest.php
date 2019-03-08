<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

require_once 'includes/TripalFields/sio__publication/sio__publication.inc';
require_once 'includes/TripalFields/sio__publication/sio__publication_formatter.inc';

class SioPublicationTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @throws \Exception
   */
  public function testLoadFunctionReturnsTheCorrectData() {
    // Get the field information needed to instantiate the field class
    $bundle = db_query('SELECT * FROM chado_bundle WHERE data_table = :data_table LIMIT 1', [
      ':data_table' => 'feature',
    ])->fetchObject();
    $field = field_info_field('sio__publication');
    $bundle_name = "bio_data_{$bundle->bundle_id}";
    $instance = field_info_instance('TripalEntity', 'sio__publication', $bundle_name);

    // Create an instance of the field
    $publication = new \sio__publication($field, $instance);

    // Mock an entity and fill it with the expected data
    $entity = new \stdClass();
    $entity->chado_record = factory('chado.feature')->create();
    $entity->sio__publication['und'] = [];

    // Create a pub association for the field to find
    $analysis = factory('chado.analysis')->create();
    chado_insert_record('analysisfeature', [
      'feature_id' => $entity->chado_record->feature_id,
      'analysis_id' => $analysis->analysis_id,
    ]);
    $pub = factory('chado.pub')->create();
    chado_insert_record('analysis_pub', [
      'pub_id' => $pub->pub_id,
      'analysis_id' => $analysis->analysis_id,
    ]);

    // Now we can test the load function
    $publication->load($entity);
    $items = $entity->sio__publication['und'];

    // Make sure we have all the data we expect
    $this->assertNotEmpty($items);

    // Get the item
    $item = $items[0]['value'];
    $this->assertArrayHasKey('TPUB:0000039', $item);
    $this->assertArrayHasKey('TPUB:0000003', $item);
    $this->assertArrayHasKey('TPUB:0000050', $item);
    $this->assertArrayHasKey('TPUB:0000047', $item);
    $this->assertArrayHasKey('TPUB:0000052', $item);
    $this->assertArrayHasKey('SBO:0000554', $item);

    $this->assertEquals($pub->title, $item['TPUB:0000039']);

    // Now test the formatter
    $formatter = new \sio__publication_formatter($field, $instance);
    $element = [];
    $entity_type = 'TripalEntity';
    $langcode = 'und';
    $display ='';
    $formatter->view($element, $entity_type, $entity, $langcode, $items, $display);

    $this->assertNotEmpty($element);
    $this->assertEquals($element[0]['#markup'], $item['TPUB:0000003']);
  }
}
