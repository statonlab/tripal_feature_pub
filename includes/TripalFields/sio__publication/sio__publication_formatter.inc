<?php

/**
 * @class
 */
class sio__publication_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Feature Publication';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sio__publication'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * @see ChadoFieldFormatter::View()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (empty($items)) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => 'There are no publications associated with this record.',
      ];
      return;
    }

    $list_items = [];
    foreach ($items as $delta => $item) {
      $value = $item['value'];
      $pub = $item['record'];
      $title = isset($value['TPUB:0000039']) ? $value['TPUB:0000039'] : '';
      $citation = isset($value['TPUB:0000003']) ? $value['TPUB:0000003'] : '';
      try {
        $entity = chado_get_record_entity_by_table('pub', $pub->pub_id);
        if ($entity) {
          $new_title = l($title, 'bio_data/' . $entity);
          // Escape anything that isn't alphanumeric
          $title = preg_replace('/([^\w])/', '\\\\$1', $title);
          $citation = preg_replace("/$title/", $new_title, $citation);
        }
        $list_items[] = $citation;
      } catch (Exception $exception) {
        watchdog('tripal_publication_feature', $exception->getMessage(), [], WATCHDOG_ERROR);
      }
    }

    $list = '';
    if (count($list_items) == 1) {
      $list = $list_items[0];
    }
    if (count($list_items) > 1) {
      $list = [
        'title' => '',
        'items' => $list_items,
        'type' => 'ol',
        'attributes' => [],
      ];
      $list = theme_item_list($list);
    }

    $element[0] = [
      '#type' => 'markup',
      '#markup' => $list,
    ];
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/
  public function settingsSummary($view_mode) {
    return '';
  }

}
