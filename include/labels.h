#ifndef LABELS_H
#define LABELS_H

typedef struct {
	char label_name[64];
} LABEL;

void labels_init();

LABEL make_label();
#endif