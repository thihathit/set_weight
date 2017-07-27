<?php
function dbselect($table, $fields=array()) {
	$return = array();
	
  $result = db_select($table, 'q')
    ->fields('q',$fields)
    ->execute();
  while ($fetch = $result->fetchAssoc()) {
  	$return[] = $fetch;
  }

  return $return;
}

function dbinsert($table, $fields=array()) {
  if (count($fields) !== 0 || empty($table)) {
    db_insert($table)
    ->fields($fields)
    ->execute();

    return TRUE;
  }
  else {
    return FALSE;
  }
}


function dbupdate($table, $fields=array(), $nid) {
  if (count($fields) !== 0 || empty($table)) {
    db_update($table)
    ->fields($fields)
    ->condition('nid',$nid,'=')
    ->execute();

    return TRUE;
  }
  else {
    return FALSE;
  }
}


function dbdelete($table, $name, $value) {
  if (!empty($table)) {
    db_delete($table)
    ->condition($name, $value,'=')
    ->execute();

    return TRUE;
  }
  else {
    return FALSE;
  }
}


function dbselectWhere($table, $fields=array(), $name, $value) {
  $return = array();
  
  $result = db_select($table, 'q')
    ->fields('q',$fields)
    ->condition($name,$value,'=')
    ->execute();
  while ($fetch = $result->fetchAssoc()) {
    $return[] = $fetch;
  }

  return $return;
}


function dbupdateWhere($table, $fields=array(), $name, $value) {
  if (count($fields) !== 0 || empty($table)) {
    db_update($table)
    ->fields($fields)
    ->condition($name,$value,'=')
    ->execute();

    return TRUE;
  }
  else {
    return FALSE;
  }
}