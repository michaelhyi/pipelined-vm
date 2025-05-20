#ifndef LOADER_H
#define LOADER_H

/**
 * Runs the loader, which loads the assembled object file passed in via args
 * into the vm.
 *
 * @param file_path string representing the path of the assembled obj file
 */
void loader_run(char *file_path);

#endif
