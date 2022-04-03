#include <stdlib.h>
#include <assert.h>

#include <include/seg.h>
#include <include/list.h>

static list_t* seg_state;

void segment_initstate() {
	if (seg_state != NULL) {
		list_free(seg_state);
	}
	seg_state = list_create();

	list_additem(seg_state, segment_create(SEG_CS, 0x1000));
	list_additem(seg_state, segment_create(SEG_DS, 0x2000));
	list_additem(seg_state, segment_create(SEG_SS, 0x3000));
	list_additem(seg_state, segment_create(SEG_FS, 0x4000));
	list_additem(seg_state, segment_create(SEG_GS, 0x5000));
}

segment_t* segment_create(segmenttype_t type, unsigned long vbase) {
	segment_t* seg = calloc(1, sizeof(segment_t));
	seg->type = type;
	seg->vbase = vbase;

	return seg;
}

segmenttype_t segment_resove(unsigned long vbase) {
	assert(seg_state != NULL);
	for (int i = 0; i < seg_state->count; i++) {
		segment_t* seg = seg_state->items[i];
		if (seg->vbase == vbase) return seg->type;
	}
	return SEG_UNKNOWN;
}

unsigned short segment_resolve_base(segmenttype_t type) {
	assert(seg_state != NULL);
	for (int i = 0; i < seg_state->count; i++) {
		segment_t* seg = seg_state->items[i];
		if (seg->type == type) return seg->vbase;
	}
	return SEG_UNKNOWN;
}
