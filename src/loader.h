#ifndef LOADER_H
#define LOADER_H

/**
 * Runs the loader, which relies on user I/O to manually input memory addresses
 * to load machine code. In the future, this loader will support the reading of
 * assembled object files.
 */
void loader_run(void);

#endif
