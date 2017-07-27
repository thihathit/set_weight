<?php
function set_weight_view_form($form, &$form_state) {
  $valid = FALSE;
  $fieldsettingname = '';

  if (isset($form_state['values'])) {
    $values = $form_state['values'];
    $changed = array();
    $message = array();

    if (isset($values['set_field_weight'])) {
      $fieldvalues = $values['set_field_weight'];
      foreach ($fieldvalues as $room) {
        $fieldsettingname = $room['fieldsetting_name'];
        if ($room['field_value'] !== $room['recent_value']) {
          $changed[] = $room;
        }
      }
      if (!empty($changed)) {
        foreach ($changed as $changedValue) {
          $nid = $changedValue['nid'];
          $fvalue = $changedValue['field_value']*1;
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

  $database = dbselect('set_weight_values');
  $arg_fieldname = arg(2);
  if (!empty($arg_fieldname) || !empty($fieldsettingname)) {
    if (!empty($fieldsettingname)) {
      $field_name = $fieldsettingname;
    }
    else {
      $field_name = $arg_fieldname;
    }
	  if (!empty($database)) {
	    $sorted_data = array();
	    // filter Nodes
	      foreach ($database as $sort_key => $sort_value) {
          if ($sort_value['field_name'] == $field_name) {
            $valid = TRUE;
          	$sorted_data[$sort_value['field_name']. '_' .$sort_value['weight']] = $sort_value;
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
              'destination'=>"admin/sw-config/$field_name/view",
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
	            '#title' => t($title),
	            '#description' => "<em>" . l('View', "node/$nid") . "," . l('edit', "node/$nid/edit",$destination) . "</em> | $nid ---- $settingfieldname",
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
	          '#type' => 'button',
	          '#value' => 'Change',
	          '#validate' => array('set_weight_validate'),
	          '#weight' => $weight,
	          '#ajax' => array(
	            'callback' => 'set_weight_submit',
	            'wrapper' => $wrappername,
	            'method' => 'replace',
	            'effect' => 'fade',
	          ),
	        );

	        return $form;
	      // Render Node form fields
	    }
	  }
  }

  if (!$valid) {
    $form['text'] = array(
      '#type' => 'markup',
      '#markup' => "Values not found.",
    );
  }

  return $form;
}


function set_weight_submit($form, &$form_state) {
  $wrappername = 'set_weight_form';

  return $form[$wrappername];
}


function set_weight_validate($form, &$form_state) {
  $wrappername = 'set_weight_form';
  if (isset($form_state['values'])) {
    $values = $form_state['values'];
    $changed = array();
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
            if ((!is_numeric($room['field_value']) || intval($room['field_value']) != $room['field_value'] || $room['field_value'] <= 0) || ($room['field_value'] == $fvalue['field_value'] && $room['nid'] !== $fvalue['nid'] && $room['fieldsetting_name'] == $fvalue['fieldsetting_name'])) {
              $invalided[] = $room['nid'];
              break;
            }
          }
          if (in_array($room['nid'], $invalided)) {
            $nid = $room['nid'];
            $fieldname = "$wrappername][set_field_weight][$nid][field_value";
            form_set_error("set_field_weight][$nid][field_value", '"'. $room['fieldtitle'] . '" must be unique and positive integer.');
          }
        } 
      }
      else {
        drupal_set_message(t('No Changes'), 'status');
      }
    }
  }
}