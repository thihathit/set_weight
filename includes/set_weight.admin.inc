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
			// if ($insert) {
			//   $messages = '<div class="messages status">Information is saved</div>';
			//   $commands = array();
			//   $loaddata = dbselect('set_weight_setting');

			//   if (!empty($loaddata)) {
			// 	  $table = sw_table($loaddata);
			//   }

			// 	$commands[] = ajax_command_replace('#set-weight-record .wrapper', '<div class="wrapper">' . $messages . $table . '</div>');
			//   $commands[] = ajax_command_replace('#sw_field_wrapper', drupal_render($form[$wrappername]));
			//   $ajax = array(
			//     '#type' => 'ajax',
			//     '#commands' => $commands
			//   );
			  
			//   return $ajax;
			// }
			drupal_set_message(t('Information is saved'), 'status');
			return $form[$wrappername];
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
		'#type' => 'submit',
		'#value' => t('Add'),
		'#weight' => $weight,
		'#ajax' => array(
      'callback' => 'set_weight_admin_submit',
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


function set_weight_admin_submit($form, &$form_state) {
	$wrappername = 'sw_field_wrapper';
	return $form[$wrappername];
}


function set_weight_admin_settings_delete($form, $form_state, $id) {
	$db = dbselectWhere('set_weight_setting',array(),'id',$id);

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


function set_weight_admin_settings_delete_submit($form, &$form_state) {
	if (isset($form_state['values']) && isset($form_state['values']['the_id'])) {
		$db = dbselectWhere('set_weight_setting',array('field_name'),'id',$form_state['values']['the_id']);
		if (!empty($db)) {
			foreach ($db as $room) {
				dbdelete('set_weight_values', 'field_name',$room['field_name']);
			}
		}
		dbdelete('set_weight_setting', 'id',$form_state['values']['the_id']);

		drupal_set_message('The information is Deleted','status');
		$form_state['redirect'] = 'admin/config/system/sw-config/';
	}
}


function sw_table($data) {
  $rows = array();
  foreach ($data as $value) {
    $rows[] = array(
      array(
        'data' => $value['type'],
        'class' => 'td-type'
      ),
      array(
        'data' => $value['field_name'],
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