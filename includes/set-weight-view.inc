<?php
function set_weight_form($form, $form_state) {
  $form_state['rebuild'] = FALSE;
  $valid = FALSE;
  
  if (isset($_GET['field_name'])) {
    $field_name = $_GET['field_name'];
    $database = dbselect('set_weight_values');
    if (!empty($database)) {
      $sorted_data = array();
      // filter Nodes
        foreach ($database as $sort_key => $sort_value) {
          if ($sort_value['field_name'] == $field_name) {
            $valid = TRUE;
            $sorted_data[$sort_value['weight']] = $sort_value;
          }
        }
        ksort($sorted_data);
      // filter Nodes
        if ($valid) {
          // Render Node form fields
            $weight = 0;
            $wrappername = 'set_weight_form';
            $destination = array(
              'query'=> array(
                'destination'=>"admin/config/system/sw-config/view?field_name=$field_name",
              ),
            );
            $form[$wrappername] = array(
              '#type' => 'markup',
              '#prefix' => '<div id=' . $wrappername . '>',
              '#suffix' => '</div>',
            );

            foreach ($sorted_data as $data_key => $data_value) {
              $title = $data_value['title'];
              $nid = $data_value['nid'];
              $dataWeight = $data_value['weight'];
              $settingfieldname = $data_value['field_name'];

              $form[$wrappername]['set_field_weight']['#tree'] = TRUE;
              $weight++;
              $form[$wrappername]['set_field_weight'][$nid]['field_value'] = array(
                '#type' => 'textfield',
                '#title' => t($title) . '|' . l($nid, "node/$nid/edit",$destination),
                '#required' => TRUE,
                '#default_value'=> $dataWeight,
                '#weight' => $weight,
              );
              $form[$wrappername]['set_field_weight'][$nid]['fieldtitle'] = array(
                '#type' => 'value',
                '#value' => $title,
              );
              $form[$wrappername]['set_field_weight'][$nid]['fieldsetting_name'] = array(
                '#type' => 'value',
                '#value' => $settingfieldname,
              );
              $form[$wrappername]['set_field_weight'][$nid]['nid'] = array(
                '#type' => 'value',
                '#value' => $nid,
              );
              $form[$wrappername]['set_field_weight'][$nid]['recent_value'] = array(
                '#type' => 'value',
                '#value' => $dataWeight,
              );
            }

            $weight++;
            $form[$wrappername]['set_field-submit'] = array(
              '#type' => 'submit',
              '#value' => 'Change',
              '#validate' => array('set_weight_validate'),
              '#weight' => $weight,
              '#submit' => array('set_weight_submit'),
            );

            return $form;
          // Render Node form fields
        }
    }
  }
  if (!$valid) {
    $form['text'] = array(
      '#type' => 'markup',
      '#markup' => "Values has not added yet.",
    );

    return $form;
  }
}


function set_weight_submit($form, $form_state) {
  $wrappername = 'set_weight_form';
  if (isset($form_state['values'])) {
    $values = $form_state['values'];
    $changed = array();
    $message = array();

    if (isset($values['set_field_weight'])) {
      $fieldvalues = $values['set_field_weight'];
      foreach ($fieldvalues as $room) {
        if ($room['field_value'] !== $room['recent_value']) {
          $changed[] = $room;
        }
      }
      if (!empty($changed)) {
        foreach ($changed as $changedValue) {
          $nid = $changedValue['nid'];
          $fvalue = $changedValue['field_value'];
          $fieldsetting_name = $changedValue['fieldsetting_name'];

          $fields = array(
            'weight' => $fvalue,
          );
          $fieldsNode = array(
            $fieldsetting_name.'_value' => $fvalue,
          );
          $update = dbupdate('set_weight_values',$fields, $nid);
          $updateNode = dbupdateWhere('field_data_'.$fieldsetting_name,$fieldsNode, 'entity_id', $nid);
          $updateRevNode = dbupdateWhere('field_revision_'.$fieldsetting_name,$fieldsNode, 'entity_id', $nid);
          field_cache_clear();
          if ($update && $updateNode && $updateRevNode) {
            $message[] = $changedValue['fieldtitle'];
          }
        } 
      }
    }

    if (!empty($message)) {
      $text = implode(', ', $message);
      drupal_set_message(t('"'.$text.'" has been updated.'), 'status');
    }
  }

  return $form;
}


function set_weight_validate($form, $form_state) {
  $wrappername = 'set_weight_form';
  if (isset($form_state['values'])) {
    $values = $form_state['values'];
    $changed = array();
    $valided = array();
    $invalided = array();

    if (isset($values['set_field_weight'])) {
      $fieldvalues = $values['set_field_weight'];
      foreach ($fieldvalues as $room) {
        if ($room['field_value'] !== $room['recent_value']) {
          $changed[] = $room;
        }
      }
      if (!empty($changed)) {
        foreach ($changed as $room) {
          foreach ($fieldvalues as $fvalue) {
            if (($room['field_value']*1 == 0) || (($room['field_value'] == $fvalue['field_value']) && ($room['nid'] !== $fvalue['nid']))) {
              $invalided[] = $room['nid'];
              break;
            }
          }
          if (!in_array($room['nid'], $invalided)) {
            $valided[] = $room;
          }
          else {
            $nid = $room['nid'];
            $fieldname = "$wrappername][set_field_weight][$nid][field_value";
            form_set_error("set_field_weight][$nid][field_value", '"'. $room['fieldtitle'] . '" must be unique and neumeric.');
          }
        } 
      }
    }
  }

  return $form;
}