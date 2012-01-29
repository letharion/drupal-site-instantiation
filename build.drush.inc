<?php

//require_once ('');

function build_drush_command() {
  $items = array();

  $items['buildinstall'] = array(
    'description' => "Build a Drupal site.",
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  return $items;
}

function drush_buildinstall_validate() {
  if (!(function_exists('drush_make') || function_exists('drush_drush_make_make'))) {
    return drush_set_error('DRUSH_MAKE_NOT_FOUND', dt('Drush make could be found'));
  }
}

function drush_buildinstall() {
  $files = array_diff(scandir('./'), array('..', '.'));
  $makefiles = array_filter($files, function($file) {
    return substr($file, -5) === '.make';
  });

  $settings = array(
    'old-dir' => 'web',
    'build-dir' => 'new-web',
    'make-files' => $makefiles
  )
  
  _build_invoke_drush_make($settings);
  _build_move_old_site($settings);
}

function _build_move_old_site($settings) {
   if (file_exists($settings['old-dir']) && !is_file($settings['old-dir'])) {
     copy('./web/sites/default/settings.php', './new-web/sites/default/settings.php');
     // Go into both sites and list modules
     // Compare lists, and run disable/uninstall on any removed ones

     // Tell Varnish that apache should not be contacted?
     // drush vset maintenance mode = 1;
     // mv actual serve directory
     rename('./web/sites/default/files', './new-web/sites/default/files');
     // Run enable on any modules added

     // drush vset maintenance mode = 0;

     rename('./web/', './old-web');
     rename('./new-web/', './web');
  }
}

function _build_invoke_drush_make($settings) {
  echo 'drush make:ing the following files: ' . implode(' ', $settings['make-files']) . "\n";
  
  foreach ($settings['make-files'] as $makefile) {
      drush_invoke('make', $makefile, $build_dir);
  }
}
