<?php

/**
 * @file
 * Holds entity module's theme functions.
 */

/**
 * Returns HTML for an entity property.
 *
 * This is the default theme implementation to display the value of a property.
 * This function can be overridden with varying levels of specificity. For
 * example, for a property named 'title' displayed on the 'article' bundle,
 * any of the following functions will override this default implementation.
 * The first of these functions that exists is used:
 * - THEMENAME_property__body__article()
 * - THEMENAME_property__article()
 * - THEMENAME_property__body()
 * - THEMENAME_property()
 *
 * @param $variables
 *   An associative array containing:
 *   - label: A boolean indicating to show or hide the property label.
 *   - title_attributes: A string containing the attributes for the title.
 *   - label: The label for the property.
 *   - content_attributes: A string containing the attributes for the content's
 *     div.
 *   - content: The rendered property value.
 *   - classes: A string containing the classes for the wrapping div.
 *   - attributes: A string containing the attributes for the wrapping div.
 *
 * @ingroup themeable
 */
function theme_entity_property($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<div class="entity-property-label"' . $variables['title_attributes'] . '>' . $variables['label'] . ':&nbsp;</div>';
  }

  // Render the content.
  $content_suffix = '';
  if (!$variables['label_hidden'] || $variables['content_attributes']) {
    $output .= '<div' . $variables['content_attributes'] . '>';
    $content_suffix = '</div>';
  }
  $output .= $variables['content'] . $content_suffix;

  // Render the top-level DIV.
  return '<div class="' . $variables['classes'] . '"' . $variables['attributes'] . '>' . $output . '</div>';
}

/**
 * Themes the exportable status of an entity.
 */
function theme_entity_status($variables) {
  $status = $variables['status'];
  $html = $variables['html'];
  if (($status & ENTITY_FIXED) == ENTITY_FIXED) {
    $label = t('Fixed');
    $help = t('The configuration is fixed and cannot be changed.');
    return $html ? "<span class='entity-status-fixed' title='$help'>" . $label . "</span>" : $label;
  }
  elseif (($status & ENTITY_OVERRIDDEN) == ENTITY_OVERRIDDEN) {
    $label = t('Overridden');
    $help = t('This configuration is provided by a module, but has been changed.');
    return $html ? "<span class='entity-status-overridden' title='$help'>" . $label . "</span>" : $label;
  }
  elseif ($status & ENTITY_IN_CODE) {
    $label = t('Default');
    $help = t('A module provides this configuration.');
    return $html ? "<span class='entity-status-default' title='$help'>" . $label . "</span>" : $label;
  }
  elseif ($status & ENTITY_CUSTOM) {
    $label = t('Custom');
    $help = t('A custom configuration by a user.');
    return $html ? "<span class='entity-status-custom' title='$help'>" . $label . "</span>" : $label;
  }
}

/**
 * Process variables for entity.tpl.php.
 */
function template_preprocess_entity(&$variables) {
  $variables['view_mode'] = $variables['elements']['#view_mode'];
  $entity_type = $variables['elements']['#entity_type'];
  $variables['entity_type'] = $entity_type;
  $entity = $variables['elements']['#entity'];
  $variables[$variables['elements']['#entity_type']] = $entity;
  $info = entity_get_info($entity_type);

  $variables['title'] = check_plain(entity_label($entity_type, $entity));

  $uri = entity_uri($entity_type, $entity);
  $variables['url'] = $uri ? url($uri['path'], $uri['options']) : FALSE;

  if (isset($variables['elements']['#page'])) {
    // If set by the caller, respect the page property.
    $variables['page'] = $variables['elements']['#page'];
  }
  else {
    // Else, try to automatically detect it.
    $variables['page'] = $uri && $uri['path'] == $_GET['q'];
  }

  // Helpful $content variable for templates.
  $variables['content'] = array();
  foreach (element_children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }

  if (!empty($info['fieldable'])) {
    // Make the field variables available with the appropriate language.
    field_attach_preprocess($entity_type, $entity, $variables['content'], $variables);
  }
  list(, , $bundle) = entity_extract_ids($entity_type, $entity);

  // Gather css classes.
  $variables['classes_array'][] = drupal_html_class('entity-' . $entity_type);
  $variables['classes_array'][] = drupal_html_class($entity_type . '-' . $bundle);

  // Add RDF type and about URI.
  if (module_exists('rdf')) {
    $variables['attributes_array']['about'] = empty($uri['path']) ? NULL: url($uri['path']);
    $variables['attributes_array']['typeof'] = empty($entity->rdf_mapping['rdftype']) ? NULL : $entity->rdf_mapping['rdftype'];
  }

  // Add suggestions.
  $variables['theme_hook_suggestions'][] = $entity_type;
  $variables['theme_hook_suggestions'][] = $entity_type . '__' . $bundle;
  $variables['theme_hook_suggestions'][] = $entity_type . '__' . $bundle . '__' . $variables['view_mode'];
  if ($id = entity_id($entity_type, $entity)) {
    $variables['theme_hook_suggestions'][] = $entity_type . '__' . $id;
  }
}
