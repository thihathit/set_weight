<?php
function set_weight_admin_setting_form($form, &$form_state) {
	$weight = 0;
	$wrappername = 'sw_field_wrapper';

  if (isset($form_state['values'])) {
		$valid = TRUE;
		$values = $form_state['values'];
		$type = $values['sw-contenttype'];
		$fieldname = $values['sw-field-name'];
		$fields = array(
			'type' => $type,
			'field_name' => $fieldname,
		);

		$select = dbselect('set_weight_setting');
		if (!empty($select)) {
			foreach ($select as $value) {
				if (in_array($type, $value) || in_array($fieldname, $value)) {
					$valid = FALSE;
				}
			}
		}
		if ($valid) {
			$insert = dbinsert('set_weight_setting',$fields);
			drupal_set_message(t('Information is saved'), 'status');
		}
	}

	$select = dbselect('set_weight_setting');
	$html = '<div id="set-weight-record">';
		$html .= '<div class="wrapper">';
			if (!empty($select)) {
				$html .= sw_table($select);
			}
		$html .= '</div>';
	$html .= '</div>';

	$form[$wrappername] = array(
		'#type' => 'markup',
		'#prefix' => '<div id=' . $wrappername . '>',
		'#suffix' => '</div>',
	);

	$weight++;
	$form[$wrappername]['sw-field-data']  = array(
		'#type' => 'markup',
		'#markup' => $html,
		'#weight' => $weight,
	);
	
	$weight++;
	$form[$wrappername]['sw-contenttype'] = array(
		'#title' => 'Content Type',
		'#type' => 'textfield',
		'#weight' => $weight,
		'#required'=> TRUE,
	);

	$weight++;
	$form[$wrappername]['sw-field-name'] = array(
		'#title' => 'Field Name',
		'#type' => 'textfield',
		'#weight' => $weight,
		'#required'=> TRUE,
	);

	$weight++;
	$form['sw-submit'] = array(
		'#type' => 'button',
		'#value' => t('Add'),
		'#weight' => $weight,
		'#ajax' => array(
      'callback' => 'set_weight_admin_ajax_callback',
      'wrapper' => $wrappername,
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#validate'=> array('set_weight_admin_validate'),
	);

	return $form;
}


function set_weight_admin_validate($form, &$form_state) {
	$valid = TRUE;
	$wrappername = 'sw_field_wrapper';
	$values = $form_state['values'];

	if (!empty($values['sw-contenttype']) || !empty($values['sw-field-name'])) {
		$type = $values['sw-contenttype'];
		$fieldname = $values['sw-field-name'];
		$fields = array(
			'type' => $type,
			'field_name' => $fieldname,
		);

		$select = dbselect('set_weight_setting');
		if (!empty($select)) {
			foreach ($select as $value) {
				if (in_array($fieldname, $value)) {
					form_set_error('sw-field-name',"Value '$fieldname' already Exists");
				}
				if (in_array($type, $value)) {
					form_set_error('sw-contenttype',"Value '$type' already Exists");
				}
			}
		}
	}
}


/*
	* Ajax submit callback from "set_weight_admin_setting_form"
*/
function set_weight_admin_ajax_callback($form, &$form_state) {
	$wrappername = 'sw_field_wrapper';
	return $form[$wrappername];
}


/*
	* Comfirm form for deleting set weight setting data
*/
function set_weight_admin_settings_delete_form($form, $form_state, $id) {
	$db = dbselectWhere('set_weight_setting',array(),'id',$id);
	if (!empty($db)) {
		$form['the_id'] = array(
			'#type' => 'value',
			'#value' => $id,
		);
		$confirm = confirm_form(
			$form,
			t('Are you sure you want to Delete "' . $db[0]['field_name'] . '" from "' . $db[0]['type'] . '"'),
			'admin/config/system/sw-config',
			t('This action cannot be undone.'),
			t('Delete'),
			t('Cancel')
		);

		return $confirm;
	}
	else {
		drupal_not_found();
		drupal_exit();
	}
}


/*
	* Comfirm Event for deleting set weight setting data
*/
function set_weight_admin_settings_delete_form_submit($form, &$form_state) {
	if (isset($form_state['values']) && isset($form_state['values']['the_id'])) {
		$db = dbselectWhere('set_weight_setting',array('field_name'),'id',$form_state['values']['the_id']);
		if (!empty($db)) {
			foreach ($db as $room) {
				dbdelete('set_weight_values', 'field_name',$room['field_name']);
			}
		}
		dbdelete('set_weight_setting', 'id',$form_state['values']['the_id']);

		drupal_set_message('The information is Deleted','status');
		$form_state['redirect'] = 'admin/config/system/sw-config';
	}
}


/*
	* Table Template for set weight setting data
*/
function sw_table($data) {
  $attr = array(
    'attributes' => array(
      'target'=>'_blank',
    ),
  );
  
  $rows = array();

  foreach ($data as $value) {
  	$fieldname = $value['field_name'];
    $rows[] = array(
      array(
        'data' => $value['type'],
        'class' => 'td-type'
      ),
      array(
        'data' => l($fieldname, "admin/sw-config/$fieldname/view" ,$attr),
        'class' => 'td-field-name'
      ),
      array(
        'data' => l('Delete', 'admin/' . $value['id'] . '/delete'),
        'class' => 'td-id'
      ),
    );
  }

  $header = array(
    'type' => array(
      'data' => t('Content Type'),
      'class' => 'th-type',
    ),
    'field_name' => array(
      'data' => t('Field Name'),
      'class' => 'th-field-name',
    ),
    'operation' => array(
      'data' => t('Operation'),
      'class' => 'th-operation',
    ),
  );

  return theme('table', array('header' => $header, 'rows' => $rows));
}