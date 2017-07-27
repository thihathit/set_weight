<?php
function validate_set_weight_values($form, &$form_state){
  $settingfieldname = array();
  $settingtype = array();
	$error_fields = array();
	$values = $form_state['values'];
  $select = dbselect('set_weight_values');
  $selectsetting = dbselect('set_weight_setting');
	if ($values['op'] == 'Save') {
	  if (!empty($selectsetting)) {
		  foreach ($selectsetting as $settingvalue) {
		  	$settingfieldname[] = $settingvalue['field_name'];
		  }
		  foreach ($selectsetting as $settingvalue) {
		  	$settingtype[] = $settingvalue['type'];
		  }
	  }
		foreach ($values as $node_key => $value) {
			if (substr($node_key, 0, 5) == "field") {
				foreach ($select as $selectvalue) {
					if (in_array($form['type']['#value'], $settingtype) && in_array($node_key, $settingfieldname)) {
						$weight = $values[$node_key]['und'][0]['value'];
						if ($selectvalue['nid'] !== $values['nid']) {
							if (($weight == $selectvalue['weight']) && ($weight*1 !== 0)) {
								$error_fields[] = array(
									'label' => $form[$node_key]['und']['#title'],
									'field_name' => $node_key,
								);
							}
						}
					}
				}
			}
		}

		if (!empty($error_fields)) {
			foreach ($error_fields as $element) {
				form_set_error($element['field_name'], 'Make sure the value of ' . t($element['label']) . ' is unique and numeric');
			}
		}
	}
}


function update_set_weight_values($node){
	$select = dbselect('set_weight_values');
	$selectsetting = dbselect('set_weight_setting');
	$isOld = FALSE;
	$field_name = '';
	foreach ($selectsetting as $settingvalue) {
		if ($settingvalue['type'] == $node->type) {
			$field_name = $settingvalue['field_name'];
			break;
		}
	}
	foreach ($select as $value) {
		if ($node->nid == $value['nid']) {
			$isOld = TRUE;
			break;
		}
	}
	if ($isOld) {
		$fieldvalue = $node->$field_name;
		$fields = array(
			'field_name'=> $field_name,
			'title' => $node->title,
			'weight' => $fieldvalue['und'][0]['value'],
		);
		$insert = dbupdate('set_weight_values',$fields, $node->nid);
	}
	else {
		$fieldvalue = $node->$field_name;
		$fields = array(
			'nid' => $node->nid,
			'field_name'=> $field_name,
			'title' => $node->title,
			'weight' => $fieldvalue['und'][0]['value'],
		);
		$insert = dbinsert('set_weight_values',$fields);
	}
}


function insert_set_weight_values($node) {
	$selectsetting = dbselect('set_weight_setting');
	$field_name = '';
	foreach ($selectsetting as $settingvalue) {
		if ($settingvalue['type'] == $node->type) {
			$field_name = $settingvalue['field_name'];
			break;
		}
	}

	$fieldvalue = $node->$field_name;
	$fields = array(
		'nid' => $node->nid,
		'field_name'=> $field_name,
		'title' => $node->title,
		'weight' => $fieldvalue['und'][0]['value'],
	);
	$insert = dbinsert('set_weight_values',$fields);
}


function delete_set_weight_values($node) {
	dbdelete('set_weight_values', 'nid',$node->nid);
}